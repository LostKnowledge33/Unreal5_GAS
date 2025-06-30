#include "DBAgent.h"
#include "SpinLock.h"
#include "Packet.h"
#include <thread>

unsigned __stdcall DB_WorkerThread(LPVOID arg)
{
	DBAgent* dbAgent_ = ( DBAgent* ) arg;
	dbAgent_->ProcessQueue();

	return 1;
}

DBAgent::DBAgent()
{
	DBFuntionDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	//DBFuntionDictionary[( int ) PACKET_ID::LOGIN_REQUEST] = &DBAgent::ProcessLogin;

	thread_handle = _beginthreadex(NULL, 0, DB_WorkerThread, this, 0, NULL);

	doubleBufferManager = &DoubleBufferManager::GetInstance();
}

DBAgent::~DBAgent()
{

}

void DBAgent::ProcessQueue()
{
	while (true)
	{
		PacketInfo* data = doubleBufferManager->GetDB_Queue().DequeueReadPacket();

		if (data == nullptr)
		{
			DoubleBufferManager::GetInstance().GetDB_Queue().SwapQueue();
		}
		else
		{
			switch (data->PacketId)
			{
			case ( UINT16 ) PACKET_ID::LOGIN_REQUEST:
				ProcessLogin(data->ClientIndex, data->pDataPtr);
				break;
			case ( UINT16 ) PACKET_ID::CHARACTER_INFO_REQUEST:
				ProcessCharacterInfo(data->ClientIndex, data->pDataPtr);
				break;
			}
		}

		if (data->pDataPtr != nullptr)
			delete[] data->pDataPtr;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		continue;
	}
}

bool DBAgent::ProcessLoginQuery(const char* query_, OUT bool& result_)
{
	if (mysql_library_init(0, NULL, NULL))
	{
		fprintf(stderr, "Can't load mysql library");
		
		goto label_return;
	}

	mysql = mysql_init(NULL);
	if (mysql_real_connect(mysql, host_name, user_name, user_password, database, port_no, NULL, flags) == NULL) {
		fprintf(stderr, "Can't connect to mysql server\n");
		fprintf(stderr, "Error %u (%s): %s\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
		mysql_close(mysql);
		
		goto label_return;
	}

	if (!mysql_query(mysql, query_))
	{
		MYSQL_ROW row;
		MYSQL_RES* result;

		result = mysql_store_result(mysql);

		if (result == NULL)
		{
			printf_s("[DB ERROR] Not Found Data \n");
			result_ = false;
		}
		else
		{
			/*
			int columnCount = mysql_num_fields(result);
			//fprintf(stdout, "\n\n>> Query results\n");
			while ((row = mysql_fetch_row(result)) != NULL)
			{
				for (int i = 0; i < columnCount; i++)
				{
					//printf_s("%s ", row[i]);
				}
				//printf_s("\n");
			}
			*/
			if (mysql_errno(mysql) != 0)
			{
				fprintf(stderr, "Can't fetch record\n");
				fprintf(stderr, "Error %u (%s): %s\n",
					mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));

				goto label_return;
			}
			else
			{
				int row_count = mysql_num_rows(result);
				if (row_count > 0)
					result_ = true;
				else
					result_ = false;
				
				mysql_close(mysql);
				mysql_library_end();
				return true;
			}
		}
		mysql_free_result(result);
	}
	else
	{
		fprintf(stderr, "Query Execute Failed \n");
		fprintf(stderr, "Error %u (%s): %s\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));

		goto label_return;
	}

label_return:
	mysql_close(mysql);
	mysql_library_end();
	return false;
}

void DBAgent::ProcessLogin(UINT32 clientIndex_, char* pPacket_)
{
	char ID[MAX_USER_ID_LEN + 1];
	char PW[MAX_USER_PW_LEN + 1];

	memcpy(ID, pPacket_ + 5, MAX_USER_ID_LEN + 1);
	memcpy(PW, pPacket_ + MAX_USER_ID_LEN + 1 + 5, MAX_USER_PW_LEN + 1);

	char query[512];
	sprintf(query, "SELECT ID, PW FROM USER_LOGIN_INFO WHERE ID = '%s' AND PW = '%s'", ID, PW);

	printf("[DB] Query : %s \n", query);

	bool LoginResult = false;
	BYTE LoginPacket = 0x00;

	if (ProcessLoginQuery(query, LoginResult))
	{
		if (LoginResult)
			LoginPacket = 0x01; // 성공
		else
			LoginPacket = 0x02; // 실패
	}
	else
	{
		LoginPacket = 0x03; // 쿼리 실패
		printf_s("[DB] Query Failed \n");
	}

	LOGIN_RESPONSE_PACKET LoginResponse;
	LoginResponse.Result = LoginPacket;
	LoginResponse.PacketId = ( UINT16 ) PACKET_ID::LOGIN_RESPONSE;
	LoginResponse.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);
	LoginResponse.Type = 0x00;
	memcpy(LoginResponse.UserID, ID, MAX_USER_ID_LEN + 1);

	PacketInfo packetInfo;
	packetInfo.ClientIndex = clientIndex_;
	packetInfo.PacketId = LoginResponse.PacketId;
	packetInfo.DataSize = LoginResponse.PacketLength;
	packetInfo.pDataPtr = new char[sizeof(LOGIN_RESPONSE_PACKET)];
	memcpy(packetInfo.pDataPtr, &LoginResponse, sizeof(LOGIN_RESPONSE_PACKET));

	DoubleBufferManager::GetInstance().GetServer_Queue().EnqueueWritePacket(packetInfo);
}

bool DBAgent::ProcessCharacterInfoQuery(const char* query_, bool& result_)
{
	if (mysql_library_init(0, NULL, NULL))
	{
		fprintf(stderr, "Can't load mysql library");

		goto label_return;
	}

	mysql = mysql_init(NULL);
	if (mysql_real_connect(mysql, host_name, user_name, user_password, database, port_no, NULL, flags) == NULL) {
		fprintf(stderr, "Can't connect to mysql server\n");
		fprintf(stderr, "Error %u (%s): %s\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
		mysql_close(mysql);

		goto label_return;
	}

	if (!mysql_query(mysql, query_))
	{
		MYSQL_ROW row;
		MYSQL_RES* result;

		result = mysql_store_result(mysql);

		if (result == NULL)
		{
			printf_s("[DB ERROR] Not Found Data \n");
			result_ = false;
		}
		else
		{
			/*
			int columnCount = mysql_num_fields(result);
			//fprintf(stdout, "\n\n>> Query results\n");
			while ((row = mysql_fetch_row(result)) != NULL)
			{
				for (int i = 0; i < columnCount; i++)
				{
					//printf_s("%s ", row[i]);
				}
				//printf_s("\n");
			}
			*/
			if (mysql_errno(mysql) != 0)
			{
				fprintf(stderr, "Can't fetch record\n");
				fprintf(stderr, "Error %u (%s): %s\n",
					mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));

				goto label_return;
			}
			else
			{
				int row_count = mysql_num_rows(result);
				if (row_count > 0)
					result_ = true;
				else
					result_ = false;

				mysql_close(mysql);
				mysql_library_end();
				return true;
			}
		}
		mysql_free_result(result);
	}
	else
	{
		fprintf(stderr, "Query Execute Failed \n");
		fprintf(stderr, "Error %u (%s): %s\n", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));

		goto label_return;
	}

label_return:
	mysql_close(mysql);
	mysql_library_end();
	return false;
}

void DBAgent::ProcessCharacterInfo(UINT32 clientIndex_, char* pPacket_)
{
	char ID[MAX_USER_ID_LEN + 1];

	memcpy(ID, pPacket_ + 5, MAX_USER_ID_LEN + 1);

	char query[512];
	sprintf(query, "SELECT ID, PW FROM USER_LOGIN_INFO WHERE ID = '%s' AND PW = '%s'", ID, PW);

	printf("[DB] Query : %s \n", query);

	bool LoginResult = false;
	BYTE LoginPacket = 0x00;

	if (ProcessCharacterInfoQuery(query, LoginResult))
	{
		if (LoginResult)
			LoginPacket = 0x01; // 성공
		else
			LoginPacket = 0x02; // 실패
	}
	else
	{
		LoginPacket = 0x03; // 쿼리 실패
		printf_s("[DB] Query Failed \n");
	}

	LOGIN_RESPONSE_PACKET LoginResponse;
	LoginResponse.Result = LoginPacket;
	LoginResponse.PacketId = ( UINT16 ) PACKET_ID::LOGIN_RESPONSE;
	LoginResponse.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);
	LoginResponse.Type = 0x00;
	memcpy(LoginResponse.UserID, ID, MAX_USER_ID_LEN + 1);

	PacketInfo packetInfo;
	packetInfo.ClientIndex = clientIndex_;
	packetInfo.PacketId = LoginResponse.PacketId;
	packetInfo.DataSize = LoginResponse.PacketLength;
	packetInfo.pDataPtr = new char[sizeof(LOGIN_RESPONSE_PACKET)];
	memcpy(packetInfo.pDataPtr, &LoginResponse, sizeof(LOGIN_RESPONSE_PACKET));

	DoubleBufferManager::GetInstance().GetServer_Queue().EnqueueWritePacket(packetInfo);
}

