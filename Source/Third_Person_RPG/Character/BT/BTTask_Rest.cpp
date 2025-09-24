// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_Rest.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Rest::UBTTask_Rest()
{
	NodeName = TEXT("Rest Movement");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Rest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
    if (!AnimInstance || !Enemy->SkillMontage) return EBTNodeResult::Failed;


    // 몽타주 실행
    AnimInstance->Montage_Play(Enemy->SkillMontage);

    // 종료 델리게이트
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBTTask_Rest::OnMontageEnded, &OwnerComp);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, Enemy->SkillMontage);

    return EBTNodeResult::InProgress;
}

void UBTTask_Rest::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return;

    Enemy->Heal(5);
}

void UBTTask_Rest::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
    if (OwnerComp)
    {
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}