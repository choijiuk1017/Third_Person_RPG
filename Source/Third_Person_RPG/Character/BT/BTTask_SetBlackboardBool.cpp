// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BT/BTTask_SetBlackboardBool.h"
#include "BTTask_SetBlackboardBool.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBlackboardBool::UBTTask_SetBlackboardBool()
{
	NodeName = TEXT("Set Blackboard Bool");
}

EBTNodeResult::Type UBTTask_SetBlackboardBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	Blackboard->SetValueAsBool(TargetKey.SelectedKeyName, bNewValue);

	return EBTNodeResult::Succeeded;
}
