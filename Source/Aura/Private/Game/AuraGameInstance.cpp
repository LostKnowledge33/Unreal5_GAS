// Created By KKD

#pragma once
#include "Game/AuraGameInstance.h"
#include "Socket/AuraSocket.h"
#include "UI/WidgetController/LoginWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


void UAuraGameInstance::Init()
{
	Super::Init();
	
	Socket = new AuraSocket(this);

	Socket->InitSocket();
	if (Socket->Connect("127.0.0.1", 8000))
	{
		UE_LOG(LogClass, Log, TEXT("Server Connected."));

		Socket->StartListen();
	}
	else
	{
		UE_LOG(LogClass, Log, TEXT("Server Connection Failed."));
	}
}

void UAuraGameInstance::SendLoginPacket(FString ID, FString PW)
{
	char buffer[126];

	memset(buffer, 0, 125);

	PACKET_HEADER header;
	header.PacketLength = 71;
	header.PacketId = (unsigned short)PACKET_ID::LOGIN_REQUEST;
	header.Type = 0;

	memcpy(buffer, &header, sizeof(PACKET_HEADER));

	memcpy(buffer + 5, TCHAR_TO_ANSI(*ID), 33);
	memcpy(buffer + 38, TCHAR_TO_ANSI(*PW), 33);

	Socket->SendData(buffer, header.PacketLength);
}

void UAuraGameInstance::RecvLoginResult(char* buffer)
{
	BYTE Result = 0;
	char ID[33];

	memcpy(ID, buffer, 33);
	memcpy(&Result, buffer + 33, sizeof(BYTE));

	switch (Result)
	{
	case 1:
		UE_LOG(LogClass, Log, TEXT("Login Success"));
		PlayerID = ID;
		SendCharacterInfoRequestPacket();
		break;
	case 2:
		UE_LOG(LogClass, Log, TEXT("Login Failed"));
		break;
	case 3:
		UE_LOG(LogClass, Log, TEXT("Server Error"));
		break;
	case 4:
		UE_LOG(LogClass, Log, TEXT("Already Logined"));
		break;
	};

	UAuraAbilitySystemLibrary::GetLoginWidgetController(this)->LoginResultDelegate.Broadcast(Result);
}

void UAuraGameInstance::SendCharacterInfoRequestPacket()
{
	char buffer[126];

	memset(buffer, 0, 125);

	PACKET_HEADER header;
	header.PacketLength = 38;
	header.PacketId = (unsigned short)PACKET_ID::CHARACTER_INFO_REQUEST;
	header.Type = 0;

	memcpy(buffer, &header, sizeof(PACKET_HEADER));
	memcpy(buffer + 5, TCHAR_TO_ANSI(*PlayerID), 33);

	Socket->SendData(buffer, header.PacketLength);
}

void UAuraGameInstance::RecvCharacterInfo(char* buffer)
{


}
