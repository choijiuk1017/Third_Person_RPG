// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:


protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
public:

};
