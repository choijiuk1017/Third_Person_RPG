// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/NPC/HiddenNPC.h"

#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"

AHiddenNPC::AHiddenNPC()
{
	bHasChoice = true;
}

void AHiddenNPC::OnYesSelected(APlayerCharacter* Player)
{
	Player->SetDialogueLine(YesConfirmText);
}

void AHiddenNPC::OnNoSelected(APlayerCharacter* Player)
{
	UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GI) return;

	const bool bUnlocked = GI->CheckAndUnlockHiddenBoss(RequiredWeaponAssets);

	if (bUnlocked)
	{
		GI->bHiddenBossUnlocked = true;


		Player->SetDialogueLine(HiddenUnlockText);
	}
	else
	{
		Player->SetDialogueLine(NeedMoreWeaponText);
	}
}