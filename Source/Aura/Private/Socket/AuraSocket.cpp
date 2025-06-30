// Created By KKD

#pragma once
#include "Socket/AuraSocket.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Game/AuraGameInstance.h"

AuraSocket::AuraSocket(TWeakObjectPtr<UAuraGameInstance> InGameInstance) : StopTaskCounter(0), AuraGameInstance(InGameInstance)
{

}

AuraSocket::~AuraSocket()
{
	Stop();

	// Socket Release
	if (ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = INVALID_SOCKET;
	}

	WSACleanup();
	if (Thread != nullptr)
	{
		delete Thread;
		Thread = nullptr;
	}
}

bool AuraSocket::InitSocket()
{
	WSADATA wsaData;

	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0) {
		return false;
	}

	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET) {
		return false;
	}

	return true;
}

bool AuraSocket::Connect(const char* pszIP, int nPort)
{
	SOCKADDR_IN ServerADDR;

	ServerADDR.sin_family = AF_INET;
	ServerADDR.sin_port = htons(nPort);
	//ServerADDR.sin_addr.s_addr = inet_addr(pszIP);
	inet_pton(AF_INET, pszIP, &ServerADDR.sin_addr.s_addr);

	int ConnectResult = connect(ServerSocket, (sockaddr*)&ServerADDR, sizeof(sockaddr));
	if (ConnectResult == SOCKET_ERROR) {
		return false;
	}

	return true;
}

void AuraSocket::CloseSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}

bool AuraSocket::Init()
{
	return true;
}

uint32 AuraSocket::Run()
{
	FPlatformProcess::Sleep(0.03);

	// Main Loop
	while (StopTaskCounter.GetValue() == 0 /* && PlayerController != nullptr */)
	{
		PACKET_ID HeaderType;
		int nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		if (nRecvLen > 0)
		{
			HeaderType = GetBufferHeader(recvBuffer);

			switch (HeaderType)
			{
			case PACKET_ID::LOGIN_RESPONSE:
				UE_LOG(LogClass, Log, TEXT("GET LOGIN RESPONSE"));
				AuraGameInstance.Get()->RecvLoginResult(recvBuffer + sizeof(PACKET_HEADER));
				break;
			case PACKET_ID::CHARACTER_INFO_RESPONSE:
				UE_LOG(LogClass, Log, TEXT("GET CHARACTER_INFO_RESPONSE"));
				AuraGameInstance.Get()->RecvCharacterInfo(recvBuffer + sizeof(PACKET_HEADER));
				break;

			default:
				break;
			}
		}
	}
	return 0;
}

void AuraSocket::Stop()
{
	StopTaskCounter.Increment();
}

void AuraSocket::Exit()
{
}

bool AuraSocket::StartListen()
{
	// 스레드 시작
	if (Thread != nullptr) return false;
	Thread = FRunnableThread::Create(this, TEXT("Thread"));
	if (Thread == nullptr)
	{
		UE_LOG(LogClass, Error, TEXT("AuraSocket Thread Create Failed"));
		return false;
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("AuraSocket Thread Create Success"));
	}
	return (Thread != nullptr);
}

void AuraSocket::StopListen()
{
	// 스레드 종료
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}

void AuraSocket::SendData(const char* buffer, int nSize)
{
	send(ServerSocket, buffer, nSize, 0);
}

PACKET_ID AuraSocket::GetBufferHeader(char* buffer)
{
	int BufferHeader = 0;
	memcpy(&BufferHeader, buffer + 2, sizeof(short));

	return (PACKET_ID)BufferHeader;
}
