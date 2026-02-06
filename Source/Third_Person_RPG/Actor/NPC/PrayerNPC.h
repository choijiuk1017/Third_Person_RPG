// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Actor/NPC.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "PrayerNPC.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API APrayerNPC : public ANPC
{
	GENERATED_BODY()
public:
	APrayerNPC();

protected:
	virtual void OnYesSelected(APlayerCharacter* Player) override;
	virtual void OnNoSelected(APlayerCharacter* Player) override;
};
