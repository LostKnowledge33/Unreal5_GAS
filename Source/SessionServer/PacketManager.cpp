#include <utility>
#include <cstring>

#include "UserManager.h"
#include "PacketManager.h"
#include "DBAgent.h"
#include "SpinLock.h"


void PacketManager::Init(const UINT32 maxClient_)
{
	mRecvFuntionDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	mRecvFuntionDictionary[(int)PACKET_ID::SYS_USER_CONNECT] = &PacketManager::ProcessUserConnect;
	mRecvFuntionDictionary[(int)PACKET_ID::SYS_USER_DISCONNECT] = &PacketManager::ProcessUserDisConnect;

	mRecvFuntionDictionary[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLogin;
	mRecvFuntionDictionary[(int)PACKET_ID::LOGIN_RESPONSE] = &PacketManager::ProcessLoginResponse;

	mRecvFuntionDictionary[( int ) PACKET_ID::LOBBY_CHARACTER_REQUEST] = &PacketManager::ProcessLobbyCharacter;
	mRecvFuntionDictionary[( int ) PACKET_ID::LOBBY_CHARACTER_RESPONSE] = &PacketManager::ProcessLobbyCharacterResponse;
					
	CreateCompent(maxClient_);

	doubleBufferManager = &DoubleBufferManager::GetInstance();
}

void PacketManager::CreateCompent(const UINT32 maxClient_)
{
	mUserManager = new UserManager;
	mUserManager->Init(maxClient_);	
}

bool PacketManager::Run()
{	
	//이 부분을 패킷 처리 부분으로 이동 시킨다.
	mIsRunProcessThread = true;
	mProcessThread = std::thread([this]() { ProcessPacket(); });
	DBAgentProcessThread = std::thread([this]() { ProcessQueue(); });

	return true;
}

void PacketManager::End()
{
	mIsRunProcessThread = false;

	if (mProcessThread.joinable())
	{
		mProcessThread.join();
	}
}

void PacketManager::ReceivePacketData(const UINT32 clientIndex_, const UINT32 size_, char* pData_)
{
	auto pUser = mUserManager->GetUserByConnIdx(clientIndex_);
	pUser->SetPacketData(size_, pData_);

	EnqueuePacketData(clientIndex_);
}

void PacketManager::ProcessQueue()
{
	while (true)
	{
		PacketInfo* data = doubleBufferManager->GetServer_Queue().DequeueReadPacket();

		if (data == nullptr)
		{
			DoubleBufferManager::GetInstance().GetServer_Queue().SwapQueue();
		}
		else
		{
			switch (data->PacketId)
			{
			case ( UINT16 ) PACKET_ID::LOGIN_RESPONSE:
				ProcessLoginResponse(data->ClientIndex, data->DataSize, data->pDataPtr);
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		continue;
	}
}

void PacketManager::ProcessPacket()
{
	while (mIsRunProcessThread)
	{
		bool isIdle = true;

		if (auto packetData = DequePacketData(); packetData.PacketId > (UINT16)PACKET_ID::SYS_END)
		{
			isIdle = false;
			ProcessRecvPacket(packetData.ClientIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
		}

		if (auto packetData = DequeSystemPacketData(); packetData.PacketId != 0)
		{
			isIdle = false;
			ProcessRecvPacket(packetData.ClientIndex, packetData.PacketId, packetData.DataSize, packetData.pDataPtr);
		}
				
		if(isIdle)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void PacketManager::EnqueuePacketData(const UINT32 clientIndex_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mInComingPacketUserIndex.push_back(clientIndex_);
}

PacketInfo PacketManager::DequePacketData()
{
	UINT32 userIndex = 0;

	{
		std::lock_guard<std::mutex> guard(mLock);
		if (mInComingPacketUserIndex.empty())
		{
			return PacketInfo();
		}

		userIndex = mInComingPacketUserIndex.front();
		mInComingPacketUserIndex.pop_front();
	}

	auto pUser = mUserManager->GetUserByConnIdx(userIndex);
	auto packetData = pUser->GetPacket();
	packetData.ClientIndex = userIndex;
	return packetData;
}

void PacketManager::PushSystemPacket(PacketInfo packet_)
{
	std::lock_guard<std::mutex> guard(mLock);
	mSystemPacketQueue.push_back(packet_);
}

PacketInfo PacketManager::DequeSystemPacketData()
{
	std::lock_guard<std::mutex> guard(mLock);
	if (mSystemPacketQueue.empty())
	{
		return PacketInfo();
	}

	PacketInfo packetData = mSystemPacketQueue.front();
	mSystemPacketQueue.pop_front();
	
	return packetData;
}

void PacketManager::ProcessRecvPacket(const UINT32 clientIndex_, const UINT16 packetId_, const UINT16 packetSize_, char* pPacket_)
{
	auto iter = mRecvFuntionDictionary.find(packetId_);
	if (iter != mRecvFuntionDictionary.end())
	{
		(this->*(iter->second))(clientIndex_, packetSize_, pPacket_);
	}

}


void PacketManager::ProcessUserConnect(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	printf("[ProcessUserConnect] clientIndex: %d\n", clientIndex_);
	auto pUser = mUserManager->GetUserByConnIdx(clientIndex_);
	pUser->Clear();
}

void PacketManager::ProcessUserDisConnect(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	printf("[ProcessUserDisConnect] clientIndex: %d\n", clientIndex_);
	ClearConnectionInfo(clientIndex_);
}

void PacketManager::ProcessLogin(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	if (LOGIN_REQUEST_PACKET_SIZE != packetSize_)
	{
		return;
	}

	LOGIN_REQUEST_PACKET* pLoginReqPacket = reinterpret_cast<LOGIN_REQUEST_PACKET*>(pPacket_);

	auto pUserID = pLoginReqPacket->UserID;
	printf("requested user id = %s\n", pUserID);

	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.PacketId = (UINT16)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);

	//여기에서 이미 접속된 유저인지 확인하고, 접속된 유저라면 실패한다.
	if (mUserManager->FindUserIndexByID(pUserID) == -1) 
	{
		DoubleBufferManager::GetInstance().GetDB_Queue().EnqueueWritePacket({ clientIndex_, ( UINT16 ) PACKET_ID::LOGIN_REQUEST, packetSize_, pPacket_ });
	}
	else 
	{
		//접속중인 유저여서 실패를 반환한다.
		loginResPacket.Result = 0x04;
		SendPacketFunc(clientIndex_, sizeof(LOGIN_RESPONSE_PACKET), (char*)&loginResPacket);
		return;
	}
}

void PacketManager::ProcessLoginResponse(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	if (LOGIN_RESPONSE_PACKET_SIZE != packetSize_)
	{
		return;
	}

	LOGIN_RESPONSE_PACKET* pLoginResponse = reinterpret_cast<LOGIN_RESPONSE_PACKET*>(pPacket_);
	char* pUserID = (( LOGIN_RESPONSE_PACKET* ) pPacket_)->UserID;

	switch (pLoginResponse->Result)
	{
	case 1:
		printf("Login Success \n");	
		mUserManager->AddUser(pUserID, clientIndex_);
		break;
	case 2:
		printf("Login Failed \n");
		break;
	case 3:
		printf("Query Failed \n");
		break;
	}

	SendPacketFunc(clientIndex_, sizeof(LOGIN_RESPONSE_PACKET), pPacket_);
}

void PacketManager::ProcessLobbyCharacter(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{
	if (CHARACTER_INFO_REQUEST_PACKET_SIZE != packetSize_)
	{
		return;
	}

	DoubleBufferManager::GetInstance().GetDB_Queue().EnqueueWritePacket({ clientIndex_, ( UINT16 ) PACKET_ID::CHARACTER_INFO_REQUEST, packetSize_, pPacket_ });
}

void PacketManager::ProcessLobbyCharacterResponse(UINT32 clientIndex_, UINT16 packetSize_, char* pPacket_)
{

}

void PacketManager::ClearConnectionInfo(INT32 clientIndex_) 
{
	auto pReqUser = mUserManager->GetUserByConnIdx(clientIndex_);
		
	if (pReqUser->GetDomainState() != User::DOMAIN_STATE::NONE) 
	{
		mUserManager->DeleteUserInfo(pReqUser);
	}
}