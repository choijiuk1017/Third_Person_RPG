// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CircleAroundPlayer.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UBTTask_CircleAroundPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_CircleAroundPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "Circle")
	float AngleStep = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Circle")
	float CircleRadius = 500.0f;
};
