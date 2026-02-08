// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/BossBattleInstance.h"

#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Third_Person_RPG/Actor/BossSequenceTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

#include "Third_Person_RPG/Instance/TPRGameInstance.h"

// Sets default values
ABossBattleInstance::ABossBattleInstance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossBattleInstance::BeginPlay()
{
	Super::BeginPlay();

	if (BossSequenceTrigger && BossSequenceTrigger->bHasClearedBoss)
	{
		Destroy();
		return;
	}

	if (BossRef)
	{
		BossRef->OnBossDied.AddDynamic(this, &ABossBattleInstance::OnBossDiedHandler);
	}
	
}

// Called every frame
void ABossBattleInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!BossRef)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), BossActorTag, FoundActors);

		if (FoundActors.Num() > 0)
		{
			ABossCharacter* FoundBoss = Cast<ABossCharacter>(FoundActors[0]);
			if (FoundBoss)
			{
				BossRef = FoundBoss;
				BossRef->OnBossDied.AddDynamic(this, &ABossBattleInstance::OnBossDiedHandler);
			}
		}
	}

	if (!BossSequenceTrigger)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), BossSequenceTag, FoundActors);

		if (FoundActors.Num() > 0)
		{
			ABossSequenceTrigger* FoundBoss = Cast<ABossSequenceTrigger>(FoundActors[0]);
			if (FoundBoss)
			{
				BossSequenceTrigger = FoundBoss;
			}
		}
	}

}

void ABossBattleInstance::OnBossDiedHandler()
{
	for (AStaticMeshActor* BlockActor : BossRoomBlock)
	{
		if (IsValid(BlockActor))
		{
			BlockActor->Destroy();
		}
	}

	BossSequenceTrigger->bHasClearedBoss = true;

	if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(GetGameInstance()))
	{
		GI->RegisterClearedBoss(BossSequenceTrigger->BossID);
	}

}

