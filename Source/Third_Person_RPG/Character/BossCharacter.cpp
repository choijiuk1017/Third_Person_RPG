// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/BossCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Third_Person_RPG/Character/EnemyAIController.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Data/SkillData.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2
// Sets default values
ABossCharacter::ABossCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEnemyAIController::StaticClass();

	GetCapsuleComponent()->SetCollisionResponseToChannel(CHANNEL_ACTION, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
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
			}
		}
	}
}

// Called every frame
void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossCharacter::BaseAttackCheck()
{
	TArray<FOverlapResult> OverlapResults;

	// 충돌 탐지를 위한 시작 지점
	FVector Start = GetActorLocation() + (GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius());

	// 충돌 탐지 끝 지점
	FVector End = Start + (GetActorForwardVector() * AttackRange);

	// 오버랩 중심점 (Capsule처럼 보이게)
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat();

	// 오버랩 영역 설정
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(AttackRange, AttackRange * 0.5f);  // Radius, HalfHeight

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

					Player->TakeDamage(BossStats.AttackPower);
				}
			}
		}
	}

	// 디버그용 캡슐 시각화
	FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, AttackRange * 0.5f, AttackRange, CapsuleRotation, DrawColor, false, 3.0f);
}

void ABossCharacter::PlayPatternMontage(int32 Index)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (!PatternDatas.IsValidIndex(Index)) return;

	CurrentPatternIndex = Index;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(PatternDatas[Index]->SkillMontage);

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABossCharacter::PatternEnd);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, PatternDatas[Index]->SkillMontage);
	}
}

void ABossCharacter::PatternEnd(UAnimMontage* Montage, bool IsEnded)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}


void ABossCharacter::SkillAttackCheckByIndex(int32 Index)
{
	if (!PatternDatas.IsValidIndex(Index)) return;

	const USkillData* SkillData = PatternDatas[Index];
	if (!SkillData) return;

	FVector SpawnLocation = GetActorLocation();

	SpawnSkillEffectByData(SkillData);

	TArray<FOverlapResult> OverlapResults;

	// 기본 값
	FVector Start = GetActorLocation();
	FVector End = Start;

	// 방향 계산
	FVector Forward = GetActorForwardVector();

	switch (SkillData->SpawnType)
	{
	case ESkillEffectSpawnType::Forward:
		Start = GetActorLocation() + Forward * 200.0f;
		End = Start + Forward * SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Self:
		Start = GetActorLocation();
		End = Start + Forward * SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Ground:
		Start = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		End = Start + Forward * SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Custom:
		// 필요시 커스텀 위치 로직 추가
		break;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	bool bHasHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		Start,
		FQuat::Identity,
		CHANNEL_ACTION,
		FCollisionShape::MakeSphere(SkillData->SkillRadius),
		Params
	);

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

					Player->TakeDamage(BossStats.AttackPower);
				}
			}
		}
	}

	// Capsule 디버그 시각화
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = SkillData->SkillRange * 0.5f;
	FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(
		GetWorld(),
		CapsuleOrigin,
		CapsuleHalfHeight,
		SkillData->SkillRadius,
		FRotationMatrix::MakeFromZ(Forward).ToQuat(),
		DrawColor,
		false,
		3.0f
	);
}


void ABossCharacter::SpawnSkillEffectByData(const USkillData* Data)
{
	if (!Data || !Data->SkillEffect) return;

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	switch (Data->SpawnType)
	{
	case ESkillEffectSpawnType::Self:
		SpawnLocation = GetActorLocation();
		break;

	case ESkillEffectSpawnType::Forward:
		SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f;
		break;

	case ESkillEffectSpawnType::Ground:
		SpawnLocation = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		break;

	case ESkillEffectSpawnType::Custom:
		// 혹시 필요하다면 SkillData에 별도 커스텀 위치 변수 추가 가능
		break;

	default:
		break;
	}

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		Data->SkillEffect,
		SpawnLocation,
		SpawnRotation
	);
}

void ABossCharacter::PlayAttackMontageByIndex(int32 Index)
{
	if (AttackMontages.IsValidIndex(Index) && GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(AttackMontages[Index]);

		// 종료 콜백
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &ABossCharacter::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontages[Index]);
	}
}

void ABossCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

	CurrentAttackStep++;

	if (CurrentAttackStep >= AttackMontages.Num())
	{
		CurrentAttackStep = 0;
		return;
	}
}

void ABossCharacter::PlayHitReactMontage()
{
	if (HitReactMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
	}
}

void ABossCharacter::RegisterAttacker(AActor* Attacker)
{
	if (!Attacker) return;
	if (bIsDead) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(Attacker))
	{
		LastAttacker = PC;
	}
}

void ABossCharacter::GrantCurrencyToKiller()
{
	if (bRewardGranted) return;
	if (!LastAttacker.IsValid()) return;
	UE_LOG(LogTemp, Warning, TEXT("LastAttacker Reward"));
	LastAttacker->AddCurrency(CurrencyReward);
	bRewardGranted = true;
}


void ABossCharacter::TakeDamage(int32 DamageAmount)
{
	int32 Defense = BossStats.Defense;

	float DamageMultiplier = 100.f / (100.f + static_cast<float>(Defense));
	float FinalDamage = DamageAmount * DamageMultiplier;

	BossStats.CurrentHP -= FinalDamage;


	if (BossStats.CurrentHP <= 0)
	{

		if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
		{
			AIController->PauseAI();
		}

		// 사망 처리
		BossStats.CurrentHP = 0;
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



		UE_LOG(LogTemp, Error, TEXT("Monster Dead"));

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
				&ABossCharacter::EndHitReact,
				HitReactDuration,
				false
			);
		}
	}



	if (HPBarWidget)
	{
		float Percent = static_cast<float>(BossStats.CurrentHP) / static_cast<float>(BossStats.MaxHP);
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

void ABossCharacter::EndHitReact()
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
