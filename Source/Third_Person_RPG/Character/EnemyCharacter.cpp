// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/EnemyCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"


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

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (HPBarWidgetClass)
		{
			HPBarWidget = CreateWidget<UHPBar>(PC, HPBarWidgetClass);
			if (HPBarWidget)
			{
				HPBarWidget->AddToViewport();
				HPBarWidget->SetVisibility(ESlateVisibility::Hidden); // 처음엔 안 보이게
			}
		}
	}
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPBarWidget && !bIsDead)
	{
		FVector WorldLocation = GetActorLocation() + FVector(0, 0, 120.f); // 머리 위 위치
		FVector2D ScreenPosition;

		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			HPBarWidget->SetPositionInViewport(ScreenPosition);
		}
	}
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

void AEnemyCharacter::RegisterAttacker(AActor* Attacker)
{
	if (!Attacker) return;
	if (bIsDead) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(Attacker))
	{
		LastAttacker = PC;
	}
}

void AEnemyCharacter::GrantCurrencyToKiller()
{
	if (bRewardGranted) return;
	if (!LastAttacker.IsValid()) return;

	LastAttacker->AddCurrency(CurrencyReward);
	bRewardGranted = true;
}


void AEnemyCharacter::TakeDamage(int32 DamageAmount)
{
	int32 Defense = EnemyStats.Defense;

	float DamageMultiplier = 100.f / (100.f + static_cast<float>(Defense));
	float FinalDamage = DamageAmount * DamageMultiplier;

	EnemyStats.CurrentHP -= FinalDamage;


	if (EnemyStats.CurrentHP <= 0)
	{
		// 사망 처리
		EnemyStats.CurrentHP = 0;
		bIsDead = true;

		GrantCurrencyToKiller();

		if (HPBarWidget)
		{
			GetWorld()->GetTimerManager().ClearTimer(HideHPBarTimerHandle);
			HPBarWidget->RemoveFromParent();
			HPBarWidget = nullptr;
		}

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

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (HitReactMontage)
		{
			AnimInstance->Montage_Play(HitReactMontage);

			if (AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(GetController()))
			{
				EnemyAI->PauseAI();
			}

			GetCharacterMovement()->DisableMovement();

			GetWorld()->GetTimerManager().SetTimer(
				HitReactTimerHandle,
				this,
				&AEnemyCharacter::EndHitReact,
				HitReactDuration,
				false
			);
		}
	}



	if (HPBarWidget)
	{
		float Percent = static_cast<float>(EnemyStats.CurrentHP) / static_cast<float>(EnemyStats.MaxHP);
		HPBarWidget->SetHPBarPercent(Percent);

		HPBarWidget->SetVisibility(ESlateVisibility::Visible);

		GetWorld()->GetTimerManager().ClearTimer(HideHPBarTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			HideHPBarTimerHandle,
			[this]()
			{
				if (HPBarWidget && !bIsDead)
				{
					HPBarWidget->SetVisibility(ESlateVisibility::Hidden);
				}
			},
			1.0f,
			false
		);
	}
}

void AEnemyCharacter::EndHitReact()
{
	if (!bIsDead)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);

		if (AEnemyAIController* EnemyAI = Cast<AEnemyAIController>(GetController()))
		{
			EnemyAI->ResumeAI();
		}
	}
}
