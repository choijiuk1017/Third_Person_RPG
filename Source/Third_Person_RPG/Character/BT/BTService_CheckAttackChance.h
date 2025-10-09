// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackChance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FActionWeight
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Action")
	FName ActionName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Action", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;
};

UCLASS()
class THIRD_PERSON_RPG_API UBTService_CheckAttackChance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckAttackChance();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ActionTypeKey;

	UPROPERTY(EditAnywhere, Category = "Action Weights")
	TArray<FActionWeight> ActionWeights;

	UPROPERTY(EditAnywhere, Category = "Action Settings", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float CheckInterval = 1.0f;
};
