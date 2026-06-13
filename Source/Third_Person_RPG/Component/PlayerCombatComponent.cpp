// PlayerCombatComponent.cpp

#include "PlayerCombatComponent.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Third_Person_RPG/Data/MMComboActionData.h"
#include "Third_Person_RPG/Data/SkillData.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerPlayer)
	{
		OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
	}

	InitializeSkillEffectPool();
}

void UPlayerCombatComponent::InitializeCombatComponent(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
}

bool UPlayerCombatComponent::CanSetWeapon() const
{
	return OwnerPlayer && OwnerPlayer->CurrentWeapon == nullptr;
}

float UPlayerCombatComponent::ConvertScalingToMultiplier(const FString& Scaling) const
{
	if (Scaling == TEXT("S")) return 1.0f;
	if (Scaling == TEXT("A")) return 0.8f;
	if (Scaling == TEXT("B")) return 0.6f;
	if (Scaling == TEXT("C")) return 0.4f;
	if (Scaling == TEXT("D")) return 0.2f;
	if (Scaling == TEXT("E")) return 0.1f;

	return 0.0f;
}

void UPlayerCombatComponent::ResetCombatStats()
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CombatStats.AttackPower =
		50 + (OwnerPlayer->CharacterAttributes.Strength * 2);

	OwnerPlayer->CombatStats.Defense =
		10 + FMath::RoundToInt(OwnerPlayer->CharacterAttributes.Strength * 1.5f);
}

void UPlayerCombatComponent::ApplyWeaponStats(ATPRWeapon* Weapon)
{
	if (!OwnerPlayer || !Weapon || !Weapon->ItemData) return;

	const UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Weapon->ItemData);
	if (!WeaponItemData) return;

	const FWeaponStatData& Stat = WeaponItemData->WeaponStats;

	ResetCombatStats();

	const float PhysicalScaling =
		ConvertScalingToMultiplier(Stat.StrengthScaling) * OwnerPlayer->CharacterAttributes.Strength +
		ConvertScalingToMultiplier(Stat.DexterityScaling) * OwnerPlayer->CharacterAttributes.Dexterity;

	const float MagicScaling =
		ConvertScalingToMultiplier(Stat.IntelligenceScaling) * OwnerPlayer->CharacterAttributes.Intelligence;

	const float FireScaling =
		ConvertScalingToMultiplier(Stat.ArcaneScaling) * OwnerPlayer->CharacterAttributes.Arcane;

	const float LightningScaling =
		ConvertScalingToMultiplier(Stat.FaithScaling) * OwnerPlayer->CharacterAttributes.Faith;

	const float HolyScaling = LightningScaling;

	int32 TotalPhysical = Stat.Physical + FMath::RoundToInt(PhysicalScaling);
	int32 TotalMagic = Stat.Magic + FMath::RoundToInt(MagicScaling);
	int32 TotalFire = Stat.Fire + FMath::RoundToInt(FireScaling);
	int32 TotalLightning = Stat.Lightning + FMath::RoundToInt(LightningScaling);
	int32 TotalHoly = Stat.Holy + FMath::RoundToInt(HolyScaling);

	const int32 EnhanceLv = GetEquippedWeaponEnhanceLevel();
	const float EnhanceMul = 1.0f + (OwnerPlayer->EnhanceDamageRatePerLevel * static_cast<float>(EnhanceLv));

	TotalPhysical = FMath::RoundToInt(static_cast<float>(TotalPhysical) * EnhanceMul);
	TotalMagic = FMath::RoundToInt(static_cast<float>(TotalMagic) * EnhanceMul);
	TotalFire = FMath::RoundToInt(static_cast<float>(TotalFire) * EnhanceMul);
	TotalLightning = FMath::RoundToInt(static_cast<float>(TotalLightning) * EnhanceMul);
	TotalHoly = FMath::RoundToInt(static_cast<float>(TotalHoly) * EnhanceMul);

	OwnerPlayer->CombatStats.AttackPower =
		TotalPhysical + TotalMagic + TotalFire + TotalLightning + TotalHoly;
}

void UPlayerCombatComponent::BasicAttack()
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->bIsInteracting || OwnerPlayer->bIsRoll || OwnerPlayer->bIsSkillActing ||
		OwnerPlayer->bIsKneeling || OwnerPlayer->bIsPopupInventory)
	{
		return;
	}

	if (OwnerPlayer->bIsDead) return;

	if (OwnerPlayer->CurrentComboCount == 0)
	{
		ComboStart();
		OwnerPlayer->bIsAttacking = true;
		return;
	}

	if (OwnerPlayer->ComboTimerHandle.IsValid())
	{
		OwnerPlayer->bHasComboInput = true;
	}
	else
	{
		OwnerPlayer->bHasComboInput = false;
	}
}

void UPlayerCombatComponent::ComboStart()
{
	if (!OwnerPlayer) return;
	if (!OwnerPlayer->TryConsumeStamina(OwnerPlayer->StaminaCost_Attack)) return;

	OwnerPlayer->CurrentComboCount = 1;
	OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UMMComboActionData* ComboData =
		OwnerPlayer->CurrentWeapon ? OwnerPlayer->WeaponComboData : OwnerPlayer->BasicComboData;

	if (!ComboData || !ComboData->ComboMontage) return;

	UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->Montage_Play(ComboData->ComboMontage, ComboData->ComnboPlayRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UPlayerCombatComponent::ComboEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboData->ComboMontage);

	OwnerPlayer->ComboTimerHandle.Invalidate();
	SetComboTimer();
}

void UPlayerCombatComponent::ComboEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CurrentComboCount = 0;
	OwnerPlayer->bHasComboInput = false;
	OwnerPlayer->bIsAttacking = false;

	OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UPlayerCombatComponent::ComboCheck()
{
	if (!OwnerPlayer) return;

	OwnerPlayer->ComboTimerHandle.Invalidate();

	auto ConsumePerHitOrStop = [this]() -> bool
		{
			if (!OwnerPlayer->bRequireStaminaForComboContinue)
			{
				if (OwnerPlayer->HasStamina(OwnerPlayer->StaminaCost_AttackPerHit))
				{
					OwnerPlayer->ConsumeStamina(OwnerPlayer->StaminaCost_AttackPerHit);
				}
				return true;
			}

			if (!OwnerPlayer->TryConsumeStamina(OwnerPlayer->StaminaCost_AttackPerHit))
			{
				OwnerPlayer->bHasComboInput = false;
				OwnerPlayer->bIsAttacking = false;
				OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				return false;
			}

			return true;
		};

	if (!OwnerPlayer->bHasComboInput) return;
	if (!ConsumePerHitOrStop()) return;

	UMMComboActionData* ComboData =
		OwnerPlayer->CurrentWeapon ? OwnerPlayer->WeaponComboData : OwnerPlayer->BasicComboData;

	if (!ComboData || !ComboData->ComboMontage) return;

	OwnerPlayer->CurrentComboCount =
		FMath::Clamp(OwnerPlayer->CurrentComboCount + 1, 1, ComboData->MaxComboCount);

	UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	const FName SectionName =
		*FString::Printf(TEXT("%s%d"), *ComboData->SectionPrefix, OwnerPlayer->CurrentComboCount);

	AnimInstance->Montage_JumpToSection(SectionName, ComboData->ComboMontage);

	SetComboTimer();
	OwnerPlayer->bHasComboInput = false;
}

void UPlayerCombatComponent::SetComboTimer()
{
	if (!OwnerPlayer) return;

	UMMComboActionData* ComboData =
		OwnerPlayer->CurrentWeapon ? OwnerPlayer->WeaponComboData : OwnerPlayer->BasicComboData;

	if (!ComboData) return;

	const int32 ComboIndex = OwnerPlayer->CurrentComboCount - 1;

	if (!ComboData->ComboFrame.IsValidIndex(ComboIndex)) return;

	const float AttackSpeedRate = 1.0f;
	const float ComboAvailableTime =
		(ComboData->ComboFrame[ComboIndex] / ComboData->FrameRate) / AttackSpeedRate;

	if (ComboAvailableTime > 0.0f)
	{
		OwnerPlayer->GetWorld()->GetTimerManager().SetTimer(
			OwnerPlayer->ComboTimerHandle,
			this,
			&UPlayerCombatComponent::ComboCheck,
			ComboAvailableTime,
			false
		);
	}
}

void UPlayerCombatComponent::BaseAttackCheck()
{
	if (!OwnerPlayer) return;
	if (OwnerPlayer->CurrentWeapon != nullptr) return;
	if (!OwnerPlayer->BasicComboData) return;

	TArray<FOverlapResult> OverlapResults;

	FVector Start =
		OwnerPlayer->GetActorLocation() +
		(OwnerPlayer->GetActorForwardVector() * OwnerPlayer->GetCapsuleComponent()->GetScaledCapsuleRadius());

	FVector End =
		Start + (OwnerPlayer->GetActorForwardVector() * OwnerPlayer->BasicComboData->AttackRange);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackOverlap), false, OwnerPlayer);

	FCollisionShape CollisionShape =
		FCollisionShape::MakeCapsule(
			OwnerPlayer->BasicComboData->AttackRadius,
			OwnerPlayer->BasicComboData->AttackRange * 0.5f
		);

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(OwnerPlayer->GetActorForwardVector()).ToQuat();

	const bool bHasHit = OwnerPlayer->GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CapsuleOrigin,
		CapsuleRotation,
		CHANNEL_ACTION,
		CollisionShape,
		Params
	);

	if (bHasHit)
	{
		TSet<AEnemyCharacter*> HitEnemies;
		TSet<ABossCharacter*> HitBosses;

		for (const FOverlapResult& Result : OverlapResults)
		{
			if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Result.GetActor()))
			{
				if (!HitEnemies.Contains(Enemy))
				{
					HitEnemies.Add(Enemy);
					Enemy->RegisterAttacker(OwnerPlayer);
					Enemy->TakeDamage(OwnerPlayer->CombatStats.AttackPower);
				}
			}

			if (ABossCharacter* Boss = Cast<ABossCharacter>(Result.GetActor()))
			{
				if (!HitBosses.Contains(Boss))
				{
					HitBosses.Add(Boss);
					Boss->RegisterAttacker(OwnerPlayer);
					Boss->TakeDamage(OwnerPlayer->CombatStats.AttackPower);
				}
			}
		}
	}

	const FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;
	DrawDebugCapsule(
		OwnerPlayer->GetWorld(),
		CapsuleOrigin,
		OwnerPlayer->BasicComboData->AttackRange * 0.5f,
		OwnerPlayer->BasicComboData->AttackRadius,
		CapsuleRotation,
		DrawColor,
		false,
		3.0f
	);
}

void UPlayerCombatComponent::SkillStart()
{
	if (!OwnerPlayer || !OwnerPlayer->SkillData) return;

	if (OwnerPlayer->bIsInteracting || OwnerPlayer->bIsRoll ||
		!OwnerPlayer->SkillData->SkillMontage || OwnerPlayer->bIsSkillActing)
	{
		return;
	}

	if (OwnerPlayer->bIsDead) return;

	if (OwnerPlayer->CombatStats.CurrentStamina < OwnerPlayer->StaminaCost_Skill) return;
	OwnerPlayer->ConsumeStamina(OwnerPlayer->StaminaCost_Skill);

	if (OwnerPlayer->bSkillConsumesFP)
	{
		const int32 FPCost = OwnerPlayer->GetCurrentSkillFPCost();

		if (FPCost > 0 && !OwnerPlayer->TryConsumeFP(FPCost))
		{
			return;
		}
	}

	OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	OwnerPlayer->bIsSkillActing = true;
	OwnerPlayer->bIsAttacking = true;

	AnimInstance->Montage_Play(
		OwnerPlayer->SkillData->SkillMontage,
		OwnerPlayer->SkillData->SkillPlayRate
	);
}

void UPlayerCombatComponent::SpawnSkillEffect()
{
	if (!OwnerPlayer || !OwnerPlayer->SkillData || !OwnerPlayer->SkillData->SkillEffect)
	{
		return;
	}

	UParticleSystemComponent* EffectComponent = GetAvailableSkillEffect();

	if (!EffectComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No available skill effect in pool."));
		return;
	}

	FVector SpawnLocation = OwnerPlayer->GetActorLocation();
	FRotator SpawnRotation = OwnerPlayer->GetActorRotation();

	switch (OwnerPlayer->SkillData->SpawnType)
	{
	case ESkillEffectSpawnType::Self:
		SpawnLocation = OwnerPlayer->GetActorLocation();
		break;

	case ESkillEffectSpawnType::Forward:
		SpawnLocation = OwnerPlayer->GetActorLocation() + OwnerPlayer->GetActorForwardVector() * 200.0f;
		break;

	case ESkillEffectSpawnType::Ground:
		SpawnLocation =
			OwnerPlayer->GetActorLocation() -
			FVector(0, 0, OwnerPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		break;

	case ESkillEffectSpawnType::Custom:
		SpawnLocation =
			OwnerPlayer->GetActorLocation() +
			OwnerPlayer->GetActorRotation().RotateVector(OwnerPlayer->SkillData->CustomSpawnOffset);
		break;

	default:
		break;
	}

	EffectComponent->SetTemplate(OwnerPlayer->SkillData->SkillEffect);
	EffectComponent->SetWorldLocation(SpawnLocation);
	EffectComponent->SetWorldRotation(SpawnRotation);
	EffectComponent->SetVisibility(true);
	EffectComponent->ActivateSystem(true);

	FTimerHandle ReturnTimerHandle;

	OwnerPlayer->GetWorld()->GetTimerManager().SetTimer(
		ReturnTimerHandle,
		FTimerDelegate::CreateUObject(
			this,
			&UPlayerCombatComponent::ReturnSkillEffectToPool,
			EffectComponent
		),
		SkillEffectLifeTime,
		false
	);
}

void UPlayerCombatComponent::SkillAttackCheck()
{
	if (!OwnerPlayer || !OwnerPlayer->SkillData) return;

	SpawnSkillEffect();

	TArray<FOverlapResult> OverlapResults;

	FVector Start = OwnerPlayer->GetActorLocation();
	FVector End = Start;
	FVector Forward = OwnerPlayer->GetActorForwardVector();

	switch (OwnerPlayer->SkillData->SpawnType)
	{
	case ESkillEffectSpawnType::Forward:
		Start = OwnerPlayer->GetActorLocation() + Forward * 200.0f;
		End = Start + Forward * OwnerPlayer->SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Self:
		Start = OwnerPlayer->GetActorLocation();
		End = Start + Forward * OwnerPlayer->SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Ground:
		Start =
			OwnerPlayer->GetActorLocation() -
			FVector(0, 0, OwnerPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		End = Start + Forward * OwnerPlayer->SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Custom:
		break;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, OwnerPlayer);

	const bool bHasHit = OwnerPlayer->GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		Start,
		FQuat::Identity,
		CHANNEL_ACTION,
		FCollisionShape::MakeSphere(OwnerPlayer->SkillData->SkillRadius),
		Params
	);

	if (bHasHit)
	{
		TSet<AEnemyCharacter*> HitEnemies;
		TSet<ABossCharacter*> HitBosses;

		for (const FOverlapResult& Result : OverlapResults)
		{
			if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Result.GetActor()))
			{
				if (!HitEnemies.Contains(Enemy))
				{
					HitEnemies.Add(Enemy);
					Enemy->RegisterAttacker(OwnerPlayer);
					Enemy->TakeDamage(OwnerPlayer->CombatStats.AttackPower);
				}
			}

			if (ABossCharacter* Boss = Cast<ABossCharacter>(Result.GetActor()))
			{
				if (!HitBosses.Contains(Boss))
				{
					HitBosses.Add(Boss);
					Boss->RegisterAttacker(OwnerPlayer);
					Boss->TakeDamage(OwnerPlayer->CombatStats.AttackPower);
				}
			}
		}
	}

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = OwnerPlayer->SkillData->SkillRange * 0.5f;
	FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(
		OwnerPlayer->GetWorld(),
		CapsuleOrigin,
		CapsuleHalfHeight,
		OwnerPlayer->SkillData->SkillRadius,
		FRotationMatrix::MakeFromZ(Forward).ToQuat(),
		DrawColor,
		false,
		3.0f
	);

	OwnerPlayer->bIsSkillActing = false;
	OwnerPlayer->bIsAttacking = false;
	OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UPlayerCombatComponent::EnableWeaponHitBox()
{
	if (OwnerPlayer && OwnerPlayer->CurrentWeapon)
	{
		OwnerPlayer->CurrentWeapon->EnableHitBox();
	}
}

void UPlayerCombatComponent::DisableWeaponHitBox()
{
	if (OwnerPlayer && OwnerPlayer->CurrentWeapon)
	{
		OwnerPlayer->CurrentWeapon->DisableHitBox();
	}
}

void UPlayerCombatComponent::SetWeapon(ATPRWeapon* NewWeapon)
{
	if (!OwnerPlayer || !NewWeapon) return;

	const FName WeaponSocket(TEXT("WeaponSocket"));

	NewWeapon->AttachToComponent(
		OwnerPlayer->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponSocket
	);

	NewWeapon->SetActorRelativeLocation(NewWeapon->RelativeLocation);
	NewWeapon->SetActorRelativeRotation(NewWeapon->RelativeRotation);
	NewWeapon->SetActorScale3D(NewWeapon->RelativeScale);
	NewWeapon->SetOwner(OwnerPlayer);

	OwnerPlayer->CurrentWeapon = NewWeapon;
	OwnerPlayer->SkillData = NewWeapon->SkillData;
	OwnerPlayer->WeaponComboData = NewWeapon->ComboData;

	ApplyWeaponStats(NewWeapon);

	if (NewWeapon->HitBox)
	{
		NewWeapon->DisableHitBox();
	}
}

void UPlayerCombatComponent::EquipWeapon(UInventoryItem* WeaponItem)
{
	if (!OwnerPlayer || !WeaponItem || !WeaponItem->ItemData) return;

	const UWeaponItemData* ItemData = Cast<UWeaponItemData>(WeaponItem->ItemData);
	if (!ItemData || !ItemData->WeaponClass) return;

	UInventoryItem* PrevEquipped =
		OwnerPlayer->InventoryComponent ? OwnerPlayer->InventoryComponent->EquippedWeaponItem : nullptr;

	if (PrevEquipped && PrevEquipped != WeaponItem)
	{
		PrevEquipped->bEquipped = false;
	}

	if (OwnerPlayer->CurrentWeapon)
	{
		OwnerPlayer->CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OwnerPlayer->CurrentWeapon->Destroy();
		OwnerPlayer->CurrentWeapon = nullptr;
		OwnerPlayer->SkillData = nullptr;
		OwnerPlayer->WeaponComboData = nullptr;
	}

	if (OwnerPlayer->InventoryComponent)
	{
		OwnerPlayer->InventoryComponent->EquippedWeaponItem = WeaponItem;
	}

	WeaponItem->bEquipped = true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATPRWeapon* NewWeapon = OwnerPlayer->GetWorld()->SpawnActor<ATPRWeapon>(
		ItemData->WeaponClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (NewWeapon)
	{
		SetWeapon(NewWeapon);
	}

	OwnerPlayer->RefreshCurrentEquipped_Weapon(WeaponItem->GetItemTexture());
}

void UPlayerCombatComponent::UnEquipWeapon(UInventoryItem* WeaponItem)
{
	if (!OwnerPlayer) return;

	UTexture2D* WeaponIcon = nullptr;

	if (OwnerPlayer->CurrentWeapon)
	{
		OwnerPlayer->CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OwnerPlayer->CurrentWeapon->Destroy();

		OwnerPlayer->CurrentWeapon = nullptr;
		OwnerPlayer->SkillData = nullptr;
		OwnerPlayer->WeaponComboData = nullptr;

		if (OwnerPlayer->InventoryComponent)
		{
			OwnerPlayer->InventoryComponent->EquippedWeaponItem = nullptr;
		}

		ResetCombatStats();
	}

	OwnerPlayer->RefreshCurrentEquipped_Weapon(WeaponIcon);
}

int32 UPlayerCombatComponent::GetEquippedWeaponEnhanceLevel() const
{
	if (!OwnerPlayer) return 0;
	if (!OwnerPlayer->InventoryComponent) return 0;
	if (!OwnerPlayer->InventoryComponent->EquippedWeaponItem) return 0;

	return OwnerPlayer->InventoryComponent->EquippedWeaponItem->EnhanceLevel;
}

float UPlayerCombatComponent::GetCurrentWeaponWeight() const
{
	if (!OwnerPlayer) return 0.f;

	if (OwnerPlayer->CurrentWeapon && OwnerPlayer->CurrentWeapon->ItemData)
	{
		if (const UWeaponItemData* WData = Cast<UWeaponItemData>(OwnerPlayer->CurrentWeapon->ItemData))
		{
			return WData->WeaponStats.Weight;
		}
	}

	return 0.f;
}

