// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BossDragon.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

ABossDragon::ABossDragon()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABossDragon::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsFly)
    {
        FlyTick(DeltaSeconds);
        return;
    }

    if (!bHasTriggeredFlyPhase && BossStats.CurrentHP <= BossStats.MaxHP * 0.5f)
    {
        bHasTriggeredFlyPhase = true;
        StartFlyPhase();
    }
}

void ABossDragon::StartFlyPhase()
{
    bIsFly = true;
    bFalling = false;

    if (AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(GetController()))
    {
        EnemyAI->PauseAI();
    }

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->StopAllMontages(0.1f);
    }

    GetCharacterMovement()->GravityScale = 0.f;
    GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    GetCharacterMovement()->RotationRate = FRotator(0, 0, 0);

    FVector Loc = GetActorLocation();
    Loc.Z += 10.f;
    SetActorLocation(Loc, false, nullptr, ETeleportType::TeleportPhysics);

    GetWorld()->GetTimerManager().SetTimer(
        FlyFallTimerHandle,
        this,
        &ABossDragon::StartFalling,
        5.0f,   
        false
    );
}

void ABossDragon::StartFalling()
{
    bFalling = true;

    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Player)
    {
        TargetFallLocation = Player->GetActorLocation();
    }
}

void ABossDragon::FlyTick(float DeltaSeconds)
{
    if (!bFalling)
    {
        AddActorWorldOffset(FVector(0, 0, FlyUpSpeed * DeltaSeconds), false);
        return;
    }

    FVector Direction3D = (TargetFallLocation - GetActorLocation()).GetSafeNormal();

    float HorizontalSpeed = 1200.f;
    FVector HorizontalMove = Direction3D;
    HorizontalMove.Z = 0; 
    HorizontalMove.Normalize();

    AddActorWorldOffset(HorizontalMove * HorizontalSpeed * DeltaSeconds, true);

    FVector FallMove = FVector(0, 0, -FallSpeed * DeltaSeconds);

    FHitResult Hit;
    AddActorWorldOffset(FallMove, true, &Hit);

    if (Hit.bBlockingHit)
    {
        OnLand(Hit.Location);
    }
}

void ABossDragon::OnLand(const FVector& LandPos)
{
    bIsFly = false;
    bFalling = false;

    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    GetCharacterMovement()->GravityScale = 1.f;
    GetCharacterMovement()->RotationRate = FRotator(0, 200.f, 0);

    if (LandAttackEffect)
    {
        FVector EffectPos = LandPos;
        EffectPos.Z -= 150.f;

        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            LandAttackEffect,
            EffectPos,
            FRotator::ZeroRotator,
            FVector(10.0f) 
        );
    }

    TArray<FOverlapResult> Overlaps;
    if (GetWorld()->OverlapMultiByChannel(
        Overlaps,
        LandPos,
        FQuat::Identity,
        CHANNEL_ACTION,
        FCollisionShape::MakeSphere(LandAttackRadius)
        ))
    {
        for (auto& O : Overlaps)
        {
            APlayerCharacter* P = Cast<APlayerCharacter>(O.GetActor());
            if (P) P->TakeDamage(LandAttackDamage);
        }
    }
    if (AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(GetController()))
    {
        EnemyAI->ResumeAI();
    }

}

void ABossDragon::StartBreath()
{
    GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
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
		0.2f,
		true
	);
}

void ABossDragon::ApplyBreathDamage()
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();

    FVector Start = GetMesh()->GetSocketLocation("FireBreathSocket");
    FTransform SocketTransform = GetMesh()->GetSocketTransform("FireBreathSocket");
    FVector Dir = SocketTransform.GetUnitAxis(EAxis::Z);
    Dir.Normalize();

    const float MaxDistance = 500.f;
    const float Radius = 100.f;
    const int Steps = 5;

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
            CHANNEL_ACTION,
            FCollisionShape::MakeSphere(Radius),
            Params
        );

        if (bHit)
        {
            for (const FOverlapResult& O : Overlaps)
            {
                APlayerCharacter* Player = Cast<APlayerCharacter>(O.GetActor());
                if (!Player) continue;

                if (DamageCooldownMap.Contains(Player))
                {
                    float LastTime = DamageCooldownMap[Player];
                    if (CurrentTime - LastTime < BreathDamageCooldown)
                    {
                        continue;
                    }
                }

                Player->TakeDamage(150.0f);

                DamageCooldownMap.Add(Player, CurrentTime);
            }
        }

        DrawDebugSphere(GetWorld(), CheckPos, Radius, 6, FColor::Red, false, 0.05f);
    }
}

void ABossDragon::EndBreath()
{
    GetCharacterMovement()->RotationRate = FRotator(0.f, 200.f, 0.f);
	GetWorld()->GetTimerManager().ClearTimer(BreathTimerHandle);

	if (BreathPSC)
	{
		BreathPSC->Deactivate();      
		BreathPSC->DestroyComponent();  
		BreathPSC = nullptr;
	}
}


