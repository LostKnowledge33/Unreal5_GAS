// Created By KKD

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoginGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ALoginGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	void GetResponseLoginResult(char* buffer);
	
};
