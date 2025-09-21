// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_Retreat.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_Retreat::UBTTask_Retreat()
{
	NodeName = TEXT("Retreat Movement");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_Retreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	APawn* Player = UGameplayStatics::GetPlayerPawn(Enemy->GetWorld(), 0);
	if (!Player) return EBTNodeResult::Failed;

	// 플레이어와 반대 방향으로 목표 위치 설정
	const float RetreatDistance = 300.f;
	FVector BackwardDir = -(Player->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
	FVector RetreatLocation = Enemy->GetActorLocation() + BackwardDir * RetreatDistance;


	// 이동 중에도 플레이어 쳐다보게 고정
	Enemy->bUseControllerRotationYaw = false;
	Enemy->GetCharacterMovement()->bOrientRotationToMovement = false;

	// 수동으로 회전 고정
	FRotator LookAtRotation = (Player->GetActorLocation() - Enemy->GetActorLocation()).Rotation();
	LookAtRotation.Pitch = 0.f;
	LookAtRotation.Roll = 0.f;
	Enemy->SetActorRotation(LookAtRotation);

	// 목표 위치로 이동
	FAIMoveRequest MoveReq;
	MoveReq.SetGoalLocation(RetreatLocation);
	MoveReq.SetAcceptanceRadius(5.f);

	FNavPathSharedPtr NavPath;
	AICon->MoveTo(MoveReq, &NavPath);

	return EBTNodeResult::Succeeded;
}

