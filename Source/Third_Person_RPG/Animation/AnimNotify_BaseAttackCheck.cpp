// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Animation/AnimNotify_BaseAttackCheck.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"

void UAnimNotify_BaseAttackCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			Player->BaseAttackCheck();
		}
	}
}