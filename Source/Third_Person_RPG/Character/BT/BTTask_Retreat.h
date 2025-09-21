// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Retreat.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UBTTask_Retreat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Retreat();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:


private:
	FTimerHandle TimerHandle;
};
