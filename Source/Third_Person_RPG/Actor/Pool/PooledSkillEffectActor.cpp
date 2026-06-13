// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Pool/PooledSkillEffectActor.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"

APooledSkillEffectActor::APooledSkillEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	RootComponent = ParticleComponent;

	ParticleComponent->bAutoActivate = false;

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void APooledSkillEffectActor::BeginPlay()
{
	Super::BeginPlay();

	DeactivateEffect();
}

void APooledSkillEffectActor::ActivateEffect(UParticleSystem* InEffect, const FVector& Location, const FRotator& Rotation)
{
	if (!InEffect || !ParticleComponent)
	{
		return;
	}

	bInUse = true;
	EffectTemplate = InEffect;

	SetActorLocation(Location);
	SetActorRotation(Rotation);
	SetActorHiddenInGame(false);

	ParticleComponent->SetTemplate(InEffect);
	ParticleComponent->ActivateSystem(true);

	GetWorldTimerManager().ClearTimer(DeactivateTimerHandle);
	GetWorldTimerManager().SetTimer(
		DeactivateTimerHandle,
		this,
		&APooledSkillEffectActor::DeactivateEffect,
		LifeTime,
		false
	);
}

void APooledSkillEffectActor::DeactivateEffect()
{
	bInUse = false;

	if (ParticleComponent)
	{
		ParticleComponent->DeactivateSystem();
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

bool APooledSkillEffectActor::IsInUse() const
{
	return bInUse;
}

UParticleSystem* APooledSkillEffectActor::GetEffectTemplate() const
{
	return EffectTemplate;
}