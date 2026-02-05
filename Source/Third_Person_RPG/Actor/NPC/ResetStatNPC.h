// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Actor/NPC.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "ResetStatNPC.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API AResetStatNPC : public ANPC
{
	GENERATED_BODY()

public:
	AResetStatNPC();

protected:

	UPROPERTY(EditAnywhere, Category = "Reset")
	FCharacterAttributes DefaultAttributes;

	virtual void OnYesSelected(APlayerCharacter* Player) override;
	virtual void OnNoSelected(APlayerCharacter* Player) override;

private:

};
