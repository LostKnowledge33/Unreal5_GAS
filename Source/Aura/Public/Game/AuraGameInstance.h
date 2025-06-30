// Created By KKD

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class AuraSocket;

struct CharacterInfo
{
	FString CharacterName;
	int32 CharacterLevel;
	int32 CharacterClass;

	int32 STR;
	int32 INT;
	int32 RES;
	int32 VIG;

	int32 HP;
	int32 MP;

	int32 Exp;
	int32 Level;
	int32 AttributePoints;

	int32 LastMapIndex;
	FVector LastPosition;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;

	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex = 0;

	UFUNCTION(BlueprintCallable)
	void SendLoginPacket(FString ID, FString PW);
	void RecvLoginResult(char* buffer);

	void SendCharacterInfoRequestPacket();
	void RecvCharacterInfo(char* buffer);

private:

	AuraSocket* Socket = nullptr;

	/*
	*	Lobby Data
	*/

	FString PlayerID;

	CharacterInfo* PlayerCharacterInfos[3];
};
