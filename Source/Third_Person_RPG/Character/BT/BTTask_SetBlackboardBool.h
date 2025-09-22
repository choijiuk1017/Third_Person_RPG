// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetBlackboardBool.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UBTTask_SetBlackboardBool : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_SetBlackboardBool();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetKey;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool bNewValue;
};
