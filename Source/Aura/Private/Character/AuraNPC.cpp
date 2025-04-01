// Created By KKD

#pragma once

#include "Character/AuraNPC.h"
#include "Aura/Aura.h"

AAuraNPC::AAuraNPC()
{
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	GetMesh()->MarkRenderStateDirty();

	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	Weapon->MarkRenderStateDirty();
	
}

void AAuraNPC::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAuraNPC::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

FDialogueData AAuraNPC::GetDialogueData_Implementation()
{
	return DialougeDatas;
}

void AAuraNPC::BeginPlay()
{
	Super::BeginPlay();
}
