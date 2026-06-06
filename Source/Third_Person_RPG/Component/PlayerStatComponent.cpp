// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerStatComponent.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Character/TPRPlayerController.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/UI/PlayerStatusWidget.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

UPlayerStatComponent::UPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerPlayer)
	{
		OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
	}
}

void UPlayerStatComponent::InitializeStatComponent(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
}

void UPlayerStatComponent::CalculateDerivedStats()
{
	if (!OwnerPlayer) return;

	OwnerPlayer->DerivedStats.MaxHP =
		300 + (OwnerPlayer->CharacterAttributes.Vigor * 50);

	OwnerPlayer->DerivedStats.MaxFP =
		50 + (OwnerPlayer->CharacterAttributes.Mind * 10);

	OwnerPlayer->DerivedStats.MaxStamina =
		80 + (OwnerPlayer->CharacterAttributes.Endurance * 10);

	OwnerPlayer->DerivedStats.MaxEquipLoad =
		30.f + (OwnerPlayer->CharacterAttributes.Endurance * 1.5f);

	OwnerPlayer->DerivedStats.Poise =
		OwnerPlayer->CharacterAttributes.Endurance * 1.2f;

	OwnerPlayer->DerivedStats.Discovery =
		OwnerPlayer->CharacterAttributes.Arcane * 1.0f;
}

void UPlayerStatComponent::InitializeCombatStats()
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CombatStats.CurrentHP = OwnerPlayer->DerivedStats.MaxHP;
	OwnerPlayer->CombatStats.CurrentFP = OwnerPlayer->DerivedStats.MaxFP;
	OwnerPlayer->CombatStats.CurrentStamina = OwnerPlayer->DerivedStats.MaxStamina;

	OwnerPlayer->CombatStats.AttackPower =
		50 + (OwnerPlayer->CharacterAttributes.Strength * 2);

	OwnerPlayer->CombatStats.Defense =
		10 + FMath::RoundToInt(OwnerPlayer->CharacterAttributes.Strength * 1.5f);

	OwnerPlayer->CombatStats.Poise = OwnerPlayer->DerivedStats.Poise;
}

void UPlayerStatComponent::RecalculateStatsAfterLevelUp(bool bRefillHPFPStamina)
{
	if (!OwnerPlayer) return;

	const int32 OldMaxHP = OwnerPlayer->DerivedStats.MaxHP;
	const int32 OldMaxFP = OwnerPlayer->DerivedStats.MaxFP;
	const int32 OldMaxStamina = OwnerPlayer->DerivedStats.MaxStamina;

	CalculateDerivedStats();

	auto ScaleKeepRatio = [](int32 Curr, int32 OldMax, int32 NewMax)
		{
			if (OldMax <= 0)
			{
				return FMath::Clamp(Curr, 0, NewMax);
			}

			const float Ratio = static_cast<float>(Curr) / static_cast<float>(OldMax);
			return FMath::Clamp(FMath::RoundToInt(Ratio * static_cast<float>(NewMax)), 0, NewMax);
		};

	if (bRefillHPFPStamina)
	{
		OwnerPlayer->CombatStats.CurrentHP = OwnerPlayer->DerivedStats.MaxHP;
		OwnerPlayer->CombatStats.CurrentFP = OwnerPlayer->DerivedStats.MaxFP;
		OwnerPlayer->CombatStats.CurrentStamina = OwnerPlayer->DerivedStats.MaxStamina;
	}
	else
	{
		OwnerPlayer->CombatStats.CurrentHP =
			ScaleKeepRatio(
				OwnerPlayer->CombatStats.CurrentHP,
				OldMaxHP,
				OwnerPlayer->DerivedStats.MaxHP
			);

		OwnerPlayer->CombatStats.CurrentFP =
			ScaleKeepRatio(
				OwnerPlayer->CombatStats.CurrentFP,
				OldMaxFP,
				OwnerPlayer->DerivedStats.MaxFP
			);

		OwnerPlayer->CombatStats.CurrentStamina =
			ScaleKeepRatio(
				OwnerPlayer->CombatStats.CurrentStamina,
				OldMaxStamina,
				OwnerPlayer->DerivedStats.MaxStamina
			);
	}

	if (OwnerPlayer->CurrentWeapon)
	{
		OwnerPlayer->ApplyWeaponStats(OwnerPlayer->CurrentWeapon);
	}
	else
	{
		OwnerPlayer->ResetCombatStats();
	}

	OwnerPlayer->CombatStats.Poise = OwnerPlayer->DerivedStats.Poise;

	if (OwnerPlayer->PlayerStatusWidgetInstance)
	{
		OwnerPlayer->PlayerStatusWidgetInstance->UpdateBarLengths(
			OwnerPlayer->DerivedStats.MaxHP,
			OwnerPlayer->DerivedStats.MaxFP,
			OwnerPlayer->DerivedStats.MaxStamina
		);

		OwnerPlayer->OnHPChanged.Broadcast(
			OwnerPlayer->CombatStats.CurrentHP,
			OwnerPlayer->DerivedStats.MaxHP
		);

		OwnerPlayer->OnFPChanged.Broadcast(
			OwnerPlayer->CombatStats.CurrentFP,
			OwnerPlayer->DerivedStats.MaxFP
		);

		OwnerPlayer->OnStaminaChanged.Broadcast(
			OwnerPlayer->CombatStats.CurrentStamina,
			OwnerPlayer->DerivedStats.MaxStamina
		);
	}
}

void UPlayerStatComponent::TakeDamage(int32 DamageAmount)
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->bIsDead) return;
	if (OwnerPlayer->CombatStats.CurrentHP <= 0) return;

	const int32 Defense = OwnerPlayer->CombatStats.Defense;
	const float DamageMultiplier = 100.f / (100.f + static_cast<float>(Defense));
	const int32 FinalDamage = FMath::Max(1, FMath::RoundToInt(DamageAmount * DamageMultiplier));

	OwnerPlayer->CombatStats.CurrentHP =
		FMath::Clamp(
			OwnerPlayer->CombatStats.CurrentHP - FinalDamage,
			0,
			OwnerPlayer->DerivedStats.MaxHP
		);

	OwnerPlayer->OnHPChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentHP,
		OwnerPlayer->DerivedStats.MaxHP
	);

	if (OwnerPlayer->CombatStats.CurrentHP <= 0)
	{
		OwnerPlayer->CombatStats.CurrentHP = 0;
		OwnerPlayer->bIsDead = true;

		UE_LOG(LogTemp, Error, TEXT("플레이어 사망"));

		OwnerPlayer->DisableInput(Cast<ATPRPlayerController>(OwnerPlayer->GetController()));

		if (OwnerPlayer->DeathScreenWidgetClass)
		{
			UUserWidget* DeathWidget =
				CreateWidget<UUserWidget>(OwnerPlayer->GetWorld(), OwnerPlayer->DeathScreenWidgetClass);

			if (DeathWidget)
			{
				DeathWidget->AddToViewport();
			}

			if (UInventoryComponent* Inventory = OwnerPlayer->FindComponentByClass<UInventoryComponent>())
			{
				if (UTPRGameInstance* GI =
					Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(OwnerPlayer->GetWorld())))
				{
					GI->CacheInventory(Inventory->GetAllItems());
					GI->CacheEquippedWeapon(Inventory->EquippedWeaponItem);
					GI->RegisterPlayerStatFromPlayer(OwnerPlayer);
				}
			}
		}

		OwnerPlayer->GetWorldTimerManager().SetTimer(
			OwnerPlayer->RespawnTimerHandle,
			OwnerPlayer,
			&APlayerCharacter::RespawnPlayer,
			6.0f,
			false
		);

		return;
	}

	OwnerPlayer->bIsRoll = false;
	OwnerPlayer->bIsAttacking = false;

	UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();
	if (AnimInstance && OwnerPlayer->HitReactMontage && !OwnerPlayer->bIsSkillActing)
	{
		AnimInstance->Montage_Play(OwnerPlayer->HitReactMontage);
	}
}

void UPlayerStatComponent::AddCurrency(int32 Amount)
{
	if (!OwnerPlayer) return;
	if (Amount <= 0) return;

	const int64 NewCurrency =
		static_cast<int64>(OwnerPlayer->Currency) + static_cast<int64>(Amount);

	OwnerPlayer->Currency =
		static_cast<int32>(FMath::Clamp<int64>(NewCurrency, 0, INT32_MAX));

	OwnerPlayer->OnCurrencyChanged.Broadcast(OwnerPlayer->Currency);
}

bool UPlayerStatComponent::SpendCurrency(int32 Amount)
{
	if (!OwnerPlayer) return false;

	if (Amount <= 0) return true;
	if (OwnerPlayer->Currency < Amount) return false;

	OwnerPlayer->Currency -= Amount;
	OwnerPlayer->OnCurrencyChanged.Broadcast(OwnerPlayer->Currency);

	return true;
}

void UPlayerStatComponent::FillSaveData(FPlayerStatSaveData& OutSaveData) const
{
	if (!OwnerPlayer) return;

	OutSaveData.BaseAttributes = OwnerPlayer->CharacterAttributes;
	OutSaveData.DerivedStats = OwnerPlayer->DerivedStats;
	OutSaveData.Currency = OwnerPlayer->Currency;
	OutSaveData.SpentCurrencyOnStats = OwnerPlayer->SpentCurrencyOnStats;
}

void UPlayerStatComponent::ApplySaveData(const FPlayerStatSaveData& InSaveData)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CharacterAttributes = InSaveData.BaseAttributes;
	OwnerPlayer->Currency = InSaveData.Currency;
	OwnerPlayer->SpentCurrencyOnStats = InSaveData.SpentCurrencyOnStats;

	CalculateDerivedStats();

	OwnerPlayer->CombatStats.CurrentHP = OwnerPlayer->DerivedStats.MaxHP;
	OwnerPlayer->CombatStats.CurrentFP = OwnerPlayer->DerivedStats.MaxFP;
	OwnerPlayer->CombatStats.CurrentStamina = OwnerPlayer->DerivedStats.MaxStamina;

	OwnerPlayer->ResetCombatStats();
	OwnerPlayer->CombatStats.Poise = OwnerPlayer->DerivedStats.Poise;

	if (OwnerPlayer->CurrentWeapon)
	{
		OwnerPlayer->ApplyWeaponStats(OwnerPlayer->CurrentWeapon);
	}
}

