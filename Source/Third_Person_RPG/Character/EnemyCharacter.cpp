// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/EnemyCharacter.h"

#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"


#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	GetCapsuleComponent()->SetCollisionResponseToChannel(CHANNEL_ACTION, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemyCharacter::BaseAttackCheck()
{
	TArray<FOverlapResult> OverlapResults;

	// 충돌 탐지를 위한 시작 지점
	FVector Start = GetActorLocation() + (GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius());

	// 충돌 탐지 끝 지점
	FVector End = Start + (GetActorForwardVector() * 70.0f);

	// 오버랩 중심점 (Capsule처럼 보이게)
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat();

	// 오버랩 영역 설정
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(70.0f, 70.0f * 0.5f);  // Radius, HalfHeight

	// 쿼리 파라미터
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackOverlap), false, this);

	// 오버랩 실행
	bool bHasHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CapsuleOrigin,
		CapsuleRotation,
		CHANNEL_ACTION,
		CollisionShape,
		Params
	);

	// 피격 판정 처리
	if (bHasHit)
	{
		TSet<APlayerCharacter*> DamagedPlayers;

		for (const FOverlapResult& Result : OverlapResults)
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(Result.GetActor()))
			{
				if (!DamagedPlayers.Contains(Player))
				{
					DamagedPlayers.Add(Player);
					UE_LOG(LogTemp, Warning, TEXT("Player Damaged via Overlap"));

					Player->TakeDamage(EnemyStats.AttackPower);
				}
			}
		}
	}

	// 디버그용 캡슐 시각화
	FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, 70.0f * 0.5f, 70.0f, CapsuleRotation, DrawColor, false, 3.0f);
}

void AEnemyCharacter::PlayHitReactMontage()
{
	if (HitReactMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
	}
}

void AEnemyCharacter::TakeDamage(int32 DamageAmount)
{
	if (EnemyStats.CurrentHP == 0)
		return;

	int32 Defense = EnemyStats.Defense;

	float DamageMultiplier = 100.f / (100.f + static_cast<float>(Defense));
	float FinalDamage = DamageAmount * DamageMultiplier;

	EnemyStats.CurrentHP -= FinalDamage;

	if (EnemyStats.CurrentHP <= 0)
	{
		// 사망 처리
		EnemyStats.CurrentHP = 0;
		bIsDead = true;

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AIController->StopMovement();
			AIController->UnPossess();
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

		USkeletalMeshComponent* MeshComponent = GetMesh();
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

		UE_LOG(LogTemp, Error, TEXT("몬스터 사망"));

		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}