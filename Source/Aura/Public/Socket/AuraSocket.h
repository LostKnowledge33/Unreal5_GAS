// Created By KKD

#pragma once
#pragma comment(lib, "ws2_32.lib")
#include "CoreMinimal.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

class UAuraGameInstance;

#define MAX_BUFFER 1024

// Buffer Define Start

enum class  PACKET_ID : unsigned short
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

// Buffer Define End


#pragma pack(push,1)
struct PACKET_HEADER
{
	unsigned short PacketLength;
	unsigned short PacketId;
	unsigned char Type;
};


/**
 * 
 */
class AURA_API AuraSocket : public FRunnable
{
public:
	AuraSocket() = delete;
	AuraSocket(TWeakObjectPtr<UAuraGameInstance> InGameInstance);
	~AuraSocket();

	bool InitSocket();
	bool Connect(const char* pszIP, int nPort);

	void CloseSocket();

	FRunnableThread* Thread = nullptr;
	FThreadSafeCounter StopTaskCounter;

	// FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	bool StartListen();
	void StopListen();

	void SendData(const char* buffer, int nSize);

private:

	TWeakObjectPtr<UAuraGameInstance> AuraGameInstance;

	SOCKET	ServerSocket;
	char 	recvBuffer[MAX_BUFFER];

	PACKET_ID GetBufferHeader(char* buffer);
};
