// Created By KKD

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NPCInterface.generated.h"

USTRUCT(BlueprintType)
struct FDialogueData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DialogueName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> DialogueMessage = TArray<FString>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* SpeakerActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* PlayerActor;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UNPCInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API INPCInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FDialogueData GetDialogueData();
};
