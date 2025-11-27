// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "BossDragon.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API ABossDragon : public ABossCharacter
{
	GENERATED_BODY()
public:
	//ABossDragon();


	UPROPERTY(BlueprintReadOnly)
	bool bIsFly;
};
