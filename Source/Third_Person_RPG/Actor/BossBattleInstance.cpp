// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/BossBattleInstance.h"

#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

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

}

