// Created By KKD


#include "UI/HUD/LoginHUD.h"

#include "Game/AuraGameInstance.h"
#include "UI/Widget/MainLoginWidget.h"
#include "UI/ViewModel/MVVM_Login.h"


void ALoginHUD::SendLoginPacket(FString ID, FString PW)
{
	GetGameInstance<UAuraGameInstance>()->SendLoginPacket(ID, PW);
}

void ALoginHUD::BeginPlay()
{
	Super::BeginPlay();

	MainLoginViewModel = NewObject<UMVVM_Login>(this, MainLoginViewModelClass);
	
	MainLoginWidget = CreateWidget<UMainLoginWidget>(GetWorld(), MainLoginWidgetClass);
	MainLoginWidget->AddToViewport();
}



