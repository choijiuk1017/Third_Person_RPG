// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTService_UpdateHP.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"

UBTService_UpdateHP::UBTService_UpdateHP()
{
    NodeName = TEXT("Update Enemy HP");
    bNotifyBecomeRelevant = true;
    bNotifyCeaseRelevant = false;
}

void UBTService_UpdateHP::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetPawn());
    if (!Enemy) return;

    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        BB->SetValueAsInt(TEXT("CurrentHP"), Enemy->EnemyStats.CurrentHP);
        BB->SetValueAsInt(TEXT("MaxHP"), Enemy->EnemyStats.MaxHP); 
    }
}