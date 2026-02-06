// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/NPC/PrayerNPC.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"

APrayerNPC::APrayerNPC()
{
	bHasChoice = true;
}

void APrayerNPC::OnYesSelected(APlayerCharacter* Player)
{
	if (!Player) return;

	Player->Prayer();
}

void APrayerNPC::OnNoSelected(APlayerCharacter* Player)
{

}