// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTService_DetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

const FName TargetKey = TEXT("Target");
const FName DistanceKey = TEXT("Distance");

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
	NodeName = TEXT("Detect Player");
	Interval = 0.3f;
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(ControlledPawn->GetWorld(), 0));
	if (!Player) return;

	const float Distance = FVector::Dist(Player->GetActorLocation(), ControlledPawn->GetActorLocation());

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetKey, Player);
		BlackboardComp->SetValueAsFloat(DistanceKey, Distance);

	}

	if (Distance <= 1000.0f)
	{
		AIController->SetFocus(Player);
	}
}
