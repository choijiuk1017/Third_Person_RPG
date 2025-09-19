// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_PlayAttackMontage.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
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

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

	if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
	{
		BB->SetValueAsBool("ShouldChaseAfterRetreat", false);
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(Enemy->GetWorld(), 0));
	if (Player)
	{
		FVector Direction = (Player->GetActorLocation() - Enemy->GetActorLocation());
		FRotator LookAtRotation = Direction.Rotation();
		LookAtRotation.Pitch = 0.f; // 위아래 각도 제거
		LookAtRotation.Roll = 0.f;
		Enemy->SetActorRotation(LookAtRotation);
	}

	Enemy->PlayAttackMontageByIndex(Enemy->CurrentAttackStep);

    return EBTNodeResult::Succeeded;
}