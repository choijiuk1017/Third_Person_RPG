// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTService_DetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
	NodeName = TEXT("Detect Player");
	Interval = 0.3f;
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!ControlledPawn) return;

    UWorld* World = ControlledPawn->GetWorld();
    if (!World) return;

    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!Player) return;

    float Distance = FVector::Dist(Player->GetActorLocation(), ControlledPawn->GetActorLocation());

    // Set Target and Distance in Blackboard
    OwnerComp.GetBlackboardComponent()->SetValueAsObject("Target", Player);
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat("Distance", Distance);
}
