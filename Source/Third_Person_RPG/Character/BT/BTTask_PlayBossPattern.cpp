// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_PlayBossPattern.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Third_Person_RPG/Character/BossCharacter.h"

UBTTask_PlayBossPattern::UBTTask_PlayBossPattern()
{
	NodeName = TEXT("Play Boss Pattern");
	bNotifyTick = true; 
}

EBTNodeResult::Type UBTTask_PlayBossPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	ABossCharacter* Boss = Cast<ABossCharacter>(AICon->GetPawn());
	if (!Boss) return EBTNodeResult::Failed;

	Boss->PlayPatternMontage(PatternIndex);
	return EBTNodeResult::InProgress ;
}

void UBTTask_PlayBossPattern::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABossCharacter* Boss = Cast<ABossCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Boss) return;

	if (!Boss->GetMesh() || !Boss->GetMesh()->GetAnimInstance()) return;

	if (!Boss->GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}