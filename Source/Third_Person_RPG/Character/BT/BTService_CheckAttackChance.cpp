// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTService_CheckAttackChance.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTService_CheckAttackChance::UBTService_CheckAttackChance()
{
	NodeName = TEXT("Check Attack Chance");
	Interval = 1.0f; 
}

void UBTService_CheckAttackChance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB || ActionWeights.Num() == 0) return;

	float TotalWeight = 0.0f;
	for (const FActionWeight& Item : ActionWeights)
	{
		TotalWeight += FMath::Max(0.0f, Item.Weight);
	}

	if (TotalWeight <= 0.0f)
		return;

	float RandValue = FMath::FRandRange(0.0f, TotalWeight);
	float Accumulated = 0.0f;

	int32 ChosenIndex = 0;
	for (int32 i = 0; i < ActionWeights.Num(); ++i)
	{
		Accumulated += ActionWeights[i].Weight;
		if (RandValue <= Accumulated)
		{
			ChosenIndex = i + 1; 
			break;
		}
	}
	const FName ActionKeyName = TEXT("ActionType");
	BB->SetValueAsInt(ActionKeyName, ChosenIndex);
}