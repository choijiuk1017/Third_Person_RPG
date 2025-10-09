// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_CircleAroundPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

UBTTask_CircleAroundPlayer::UBTTask_CircleAroundPlayer()
{
	NodeName = TEXT("Circle Around Player");
}

EBTNodeResult::Type UBTTask_CircleAroundPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("Target")));
	if (!Target) return EBTNodeResult::Failed;

	float Direction = 1.0f;
	if (BB->IsVectorValueSet(TEXT("CircleDirection")))
	{
		Direction = BB->GetValueAsFloat(TEXT("CircleDirection"));
		if (Direction == 0) Direction = 1.0f;
	}

	// 타깃과의 방향
	FVector ToTarget = AIPawn->GetActorLocation() - Target->GetActorLocation();
	ToTarget.Z = 0.0f;
	ToTarget.Normalize();

	// 지정된 각도만큼 회전
	float RotAngle = AngleStep * Direction;
	FRotator Rot = FRotator(0.f, RotAngle, 0.f);
	FVector RotatedDir = Rot.RotateVector(ToTarget);

	// 새 목표 위치 계산
	FVector NewPos = Target->GetActorLocation() + RotatedDir * CircleRadius;

	// 네비게이션 가능한 위치 보정
	FNavLocation NavLoc;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys && NavSys->GetRandomPointInNavigableRadius(NewPos, 100.f, NavLoc))
	{
		AICon->MoveToLocation(NavLoc.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}