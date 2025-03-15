// Create By KKD

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	// Function	
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	// Function END

	
	// Properties
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	// Properties END
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
