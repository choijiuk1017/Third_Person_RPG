// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_PlayAttackMontage.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PlayAttackMontage::UBTTask_PlayAttackMontage()
{
	NodeName = TEXT("Play Attack Montage");
}

EBTNodeResult::Type UBTTask_PlayAttackMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AActor* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ControlledPawn);
    ABossCharacter* Boss = Cast<ABossCharacter>(ControlledPawn);
    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(ControlledPawn->GetWorld(), 0));

    if (Enemy)
    {
        // 일반 몬스터 공격
        if (Player)
        {
            FVector Direction = (Player->GetActorLocation() - Enemy->GetActorLocation());
            FRotator LookAtRotation = Direction.Rotation();
            LookAtRotation.Pitch = 0.f;
            LookAtRotation.Roll = 0.f;
            Enemy->SetActorRotation(LookAtRotation);
        }
        Enemy->PlayAttackMontageByIndex(Enemy->CurrentAttackStep);
    }
    else if (Boss)
    {
        // 보스 공격
        if (Player)
        {
            FVector Direction = (Player->GetActorLocation() - Boss->GetActorLocation());
            FRotator LookAtRotation = Direction.Rotation();
            LookAtRotation.Pitch = 0.f;
            LookAtRotation.Roll = 0.f;
            Boss->SetActorRotation(LookAtRotation);
        }
        Boss->PlayAttackMontageByIndex(Boss->CurrentAttackStep);
    }

    return EBTNodeResult::Succeeded;
}