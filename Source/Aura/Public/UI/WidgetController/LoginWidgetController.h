// Created By KKD

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "LoginWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoginResultSignature, uint8, result);

/**
 * 
 */

UCLASS()
class AURA_API ULoginWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:

	//virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FLoginResultSignature LoginResultDelegate;
	

};
