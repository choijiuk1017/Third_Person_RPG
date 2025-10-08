// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"



void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
	if (Enemy && Enemy->BehaviorTreeAsset)
	{
		RunBehaviorTree(Enemy->BehaviorTreeAsset);

		CachedBehaviorTree = Cast<UBehaviorTreeComponent>(BrainComponent);
	}

	ABossCharacter* Boss = Cast<ABossCharacter>(InPawn);
	if (Boss && Boss->BehaviorTreeAsset)
	{
		RunBehaviorTree(Boss->BehaviorTreeAsset);

		CachedBehaviorTree = Cast<UBehaviorTreeComponent>(BrainComponent);
	}
}

void AEnemyAIController::PauseAI()
{
	if (CachedBehaviorTree)
	{
		CachedBehaviorTree->PauseLogic(TEXT("HitReact"));
	}
}

void AEnemyAIController::ResumeAI()
{
	if (CachedBehaviorTree)
	{
		CachedBehaviorTree->ResumeLogic(TEXT("EndHitReact"));
	}
}