// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/NPC/ResetStatNPC.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"

AResetStatNPC::AResetStatNPC()
{
	bHasChoice = true;
}

void AResetStatNPC::OnYesSelected(APlayerCharacter* Player)
{
	if (!Player) return;

	Player->CharacterAttributes = DefaultAttributes;

	Player->RecalculateStatsAfterLevelUp(true);

	Player->AddCurrency(Player->SpentCurrencyOnStats);
	Player->SpentCurrencyOnStats = 0;
}

void AResetStatNPC::OnNoSelected(APlayerCharacter* Player)
{

}