// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BossDragon.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

void ABossDragon::StartBreath()
{
	if (!BreathEffect) return;

	USkeletalMeshComponent* MeshComp = GetMesh();
	const FName SocketName = "FireBreathSocket";

	BreathPSC = UGameplayStatics::SpawnEmitterAttached(
		BreathEffect,
		MeshComp,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget
	);

	GetWorld()->GetTimerManager().SetTimer(
		BreathTimerHandle,
		this,
		&ABossDragon::ApplyBreathDamage,
		0.5f,
		true
	);
}

void ABossDragon::ApplyBreathDamage()
{
	FVector Start = GetMesh()->GetSocketLocation("FireBreathSocket");
	FTransform SocketTransform = GetMesh()->GetSocketTransform("FireBreathSocket");
	FVector Dir = SocketTransform.GetUnitAxis(EAxis::Z);
	Dir.Normalize();

	const float MaxDistance = 500.f;     // 브레스 길이
	const float Radius = 100.f;          // 브레스 너비
	const int   Steps = 5;               // 스피어 몇 번 찍을지

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	for (int i = 0; i < Steps; i++)
	{
		float Alpha = (float)i / (Steps - 1);
		FVector CheckPos = Start + Dir * (MaxDistance * Alpha);

		Overlaps.Reset();

		bool bHit = GetWorld()->OverlapMultiByChannel(
			Overlaps,
			CheckPos,
			FQuat::Identity,
			ECC_GameTraceChannel2,
			FCollisionShape::MakeSphere(Radius),
			Params
		);

		if (bHit)
		{
			for (const FOverlapResult& O : Overlaps)
			{
				APlayerCharacter* Player = Cast<APlayerCharacter>(O.GetActor());
				if (!Player) continue;

				Player->TakeDamage(1);
			}
		}

		// Debug
		DrawDebugSphere(GetWorld(), CheckPos, Radius, 4, FColor::Red, false, 0.05f);
	}
}

void ABossDragon::EndBreath()
{
	GetWorld()->GetTimerManager().ClearTimer(BreathTimerHandle);

	if (BreathPSC)
	{
		BreathPSC->Deactivate();        // 파티클 emission 중지
		BreathPSC->DestroyComponent();  // 컴포넌트 제거
		BreathPSC = nullptr;
	}
}

