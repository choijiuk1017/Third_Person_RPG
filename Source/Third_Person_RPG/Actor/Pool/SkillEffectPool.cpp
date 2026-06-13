// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Pool/SkillEffectPool.h"

#include "PooledSkillEffectActor.h"
#include "Third_Person_RPG/Data/SkillData.h"
#include "Engine/World.h"

ASkillEffectPool::ASkillEffectPool()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASkillEffectPool::BeginPlay()
{
	Super::BeginPlay();

	if (!PooledEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("PooledEffectClass is not set."));
		return;
	}

	for (int32 i = 0; i < PoolSize; i++)
	{
		APooledSkillEffectActor* EffectActor = GetWorld()->SpawnActor<APooledSkillEffectActor>(
			PooledEffectClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator
		);

		if (EffectActor)
		{
			EffectActor->DeactivateEffect();
			EffectPool.Add(EffectActor);
		}
	}
}

APooledSkillEffectActor* ASkillEffectPool::GetAvailableEffect()
{
	for (APooledSkillEffectActor* EffectActor : EffectPool)
	{
		if (EffectActor && !EffectActor->IsInUse())
		{
			return EffectActor;
		}
	}

	return nullptr;
}

void ASkillEffectPool::PlaySkillEffect(AActor* OwnerActor, USkillData* SkillData)
{
	if (!OwnerActor || !SkillData || !SkillData->SkillEffect)
	{
		return;
	}

	APooledSkillEffectActor* EffectActor = GetAvailableEffect();

	if (!EffectActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill Effect Pool is full."));
		return;
	}

	const FVector SpawnLocation = CalculateSpawnLocation(OwnerActor, SkillData);
	const FRotator SpawnRotation = OwnerActor->GetActorRotation();

	EffectActor->ActivateEffect(SkillData->SkillEffect, SpawnLocation, SpawnRotation);
}

FVector ASkillEffectPool::CalculateSpawnLocation(AActor* OwnerActor, USkillData* SkillData) const
{
	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector Forward = OwnerActor->GetActorForwardVector();

	switch (SkillData->SpawnType)
	{
	case ESkillEffectSpawnType::Self:
		return OwnerLocation;

	case ESkillEffectSpawnType::Forward:
		return OwnerLocation + Forward * SkillData->SkillRange;

	case ESkillEffectSpawnType::Ground:
		return OwnerLocation + Forward * SkillData->SkillRange - FVector(0.0f, 0.0f, 80.0f);

	case ESkillEffectSpawnType::Custom:
		return OwnerLocation + OwnerActor->GetActorRotation().RotateVector(SkillData->CustomSpawnOffset);

	default:
		return OwnerLocation;
	}
}