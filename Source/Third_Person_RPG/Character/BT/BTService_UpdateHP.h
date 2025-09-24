// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateHP.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UBTService_UpdateHP : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateHP();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
