// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayBossPattern.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UBTTask_PlayBossPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PlayBossPattern();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// 실행할 패턴 인덱스 (정적 설정 시)
	UPROPERTY(EditAnywhere, Category = "Pattern")
	int32 PatternIndex = 0;
	
};
