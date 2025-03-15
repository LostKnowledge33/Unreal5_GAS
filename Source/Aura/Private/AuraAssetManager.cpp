// Create By KKD


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine)
	
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading(); 

	FAuraGameplayTags::InitializeNativeGameplayTags();

	// TargetData 사용시 호출 필요
	UAbilitySystemGlobals::Get().InitGlobalData();
}
