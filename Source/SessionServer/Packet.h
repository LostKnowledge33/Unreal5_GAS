#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct RawPacketData
{
	UINT32 ClientIndex = 0;
	UINT32 DataSize = 0;
	char* pPacketData = nullptr;

	void Set(RawPacketData& vlaue)
	{
		ClientIndex = vlaue.ClientIndex;
		DataSize = vlaue.DataSize;

		pPacketData = new char[vlaue.DataSize];
		CopyMemory(pPacketData, vlaue.pPacketData, vlaue.DataSize);
	}

	void Set(UINT32 clientIndex_, UINT32 dataSize_, char* pData)
	{
		ClientIndex = clientIndex_;
		DataSize = dataSize_;

		pPacketData = new char[dataSize_];
		CopyMemory(pPacketData, pData, dataSize_);
	}

	void Release()
	{
		delete pPacketData;
	}
};


struct PacketInfo
{
	UINT32 ClientIndex = 0;
	UINT16 PacketId = 0;
	UINT16 DataSize = 0;
	char* pDataPtr = nullptr;
};


enum class  PACKET_ID : UINT16
{
	//SYSTEM
	SYS_USER_CONNECT = 11,
	SYS_USER_DISCONNECT = 12,
	SYS_END = 30,

	//DB
	DB_END = 199,

	//Client
	LOGIN_REQUEST = 201,
	LOGIN_RESPONSE = 202,

	CHARACTER_INFO_REQUEST = 205,
	CHARACTER_INFO_RESPONSE = 206,
};


#pragma pack(push,1)
struct PACKET_HEADER
{
	UINT16 PacketLength;
	UINT16 PacketId;
	UINT8 Type;
};

const UINT32 PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);


const int MAX_USER_ID_LEN = 32;
const int MAX_USER_PW_LEN = 32;

struct LOGIN_REQUEST_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1];
	char UserPW[MAX_USER_PW_LEN + 1];
};
const size_t LOGIN_REQUEST_PACKET_SIZE = sizeof(LOGIN_REQUEST_PACKET);


struct LOGIN_RESPONSE_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1];
	BYTE Result;
};
const size_t LOGIN_RESPONSE_PACKET_SIZE = sizeof(LOGIN_RESPONSE_PACKET);

struct CHARACTER_INFO_REQUEST_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1];
};
const size_t CHARACTER_INFO_REQUEST_PACKET_SIZE = sizeof(CHARACTER_INFO_REQUEST_PACKET);

struct CHARACTER_INFO_RESPONSE_PACKET : public PACKET_HEADER
{
	char	CharacterName[MAX_USER_ID_LEN + 1];
	int		CharacterLevel;
	int		CharacterClass;

	int	STR;
	int	INT;
	int	RES;
	int	VIG;

	int	HP;
	int	MP;
		
	int	Exp;
	int	Level;
	int	AttributePoints;

	int	LastMapIndex;
	char LastPosition[32];
};
const size_t CHARACTER_INFO_RESPONSE_PACKET_SIZE = sizeof(CHARACTER_INFO_RESPONSE_PACKET);

#pragma pack(pop)

