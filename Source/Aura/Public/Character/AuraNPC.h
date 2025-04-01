// Created By KKD

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/NPCInterface.h"
#include "AuraNPC.generated.h"



/**
 * 
 */
UCLASS()
class AURA_API AAuraNPC : public AAuraCharacterBase, public IHighlightInterface, public INPCInterface
{
	GENERATED_BODY()
	
public:
	AAuraNPC();
	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** end Highlight Interface */

	/**		START NPCInterface	**/
	virtual FDialogueData GetDialogueData_Implementation() override;
	/**		END NPCInterface	**/

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FDialogueData DialougeDatas;
};
