// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "LastBoss.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API ALastBoss : public ABossCharacter
{
	GENERATED_BODY()
public:
	ALastBoss();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UStaticMeshComponent* WeaponMesh;
};
