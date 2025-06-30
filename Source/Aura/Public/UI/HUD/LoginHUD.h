// Created By KKD

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoginHUD.generated.h"

/**
 * 
 */

class UMainLoginWidget;
class UMVVM_Login;

UCLASS()
class AURA_API ALoginHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainLoginWidgetClass;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMainLoginWidget> MainLoginWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_Login> MainLoginViewModelClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_Login> MainLoginViewModel;
	

	UFUNCTION(BlueprintCallable)
	void SendLoginPacket(FString ID, FString PW);

protected:
	virtual void BeginPlay() override;
	
};
