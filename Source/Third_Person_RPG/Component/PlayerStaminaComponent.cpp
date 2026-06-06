// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerStaminaComponent.h"


#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/UI/PlayerStatusWidget.h"

UPlayerStaminaComponent::UPlayerStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerPlayer)
	{
		OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
	}
}

void UPlayerStaminaComponent::InitializeStaminaComponent(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
}

bool UPlayerStaminaComponent::HasStamina(int32 Amount) const
{
	if (!OwnerPlayer) return false;

	return OwnerPlayer->CombatStats.CurrentStamina >= Amount;
}

bool UPlayerStaminaComponent::TryConsumeStamina(int32 Amount)
{
	if (!HasStamina(Amount)) return false;

	ConsumeStamina(Amount);
	return true;
}

void UPlayerStaminaComponent::ConsumeStamina(int32 Amount)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CombatStats.CurrentStamina =
		FMath::Clamp(
			OwnerPlayer->CombatStats.CurrentStamina - Amount,
			0,
			OwnerPlayer->DerivedStats.MaxStamina
		);

	OwnerPlayer->OnStaminaChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentStamina,
		OwnerPlayer->DerivedStats.MaxStamina
	);
}

void UPlayerStaminaComponent::RestoreStaminaTick(int32 AmountPerTick)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CombatStats.CurrentStamina =
		FMath::Clamp(
			OwnerPlayer->CombatStats.CurrentStamina + AmountPerTick,
			0,
			OwnerPlayer->DerivedStats.MaxStamina
		);

	OwnerPlayer->OnStaminaChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentStamina,
		OwnerPlayer->DerivedStats.MaxStamina
	);
}

int32 UPlayerStaminaComponent::GetStaminaRegenPerSecond() const
{
	if (!OwnerPlayer) return 0;

	const float Half = OwnerPlayer->DerivedStats.MaxEquipLoad * 0.5f;
	const float W = OwnerPlayer->GetCurrentWeaponWeight();

	if (W <= Half - 5.f)
	{
		return OwnerPlayer->StaminaRegen_Light;
	}

	if (W > Half - 5.f && W < Half + 5.f)
	{
		return OwnerPlayer->StaminaRegen_Medium;
	}

	return OwnerPlayer->StaminaRegen_Heavy;
}

void UPlayerStaminaComponent::StaminaRegenTick(float DeltaSeconds)
{
	if (!OwnerPlayer) return;

	if (!OwnerPlayer->bEnableFrameStaminaRegen) return;

	if (OwnerPlayer->bBlockRegenWhileSprinting && OwnerPlayer->bIsSprinting) return;

	if (OwnerPlayer->CombatStats.CurrentStamina >= OwnerPlayer->DerivedStats.MaxStamina) return;

	const int32 RegenPerSec = GetStaminaRegenPerSecond();
	if (RegenPerSec <= 0) return;

	OwnerPlayer->StaminaRegenAccum += static_cast<float>(RegenPerSec) * DeltaSeconds;

	const int32 RegenWhole = FMath::FloorToInt(OwnerPlayer->StaminaRegenAccum);

	if (RegenWhole > 0)
	{
		OwnerPlayer->StaminaRegenAccum -= static_cast<float>(RegenWhole);
		RestoreStaminaTick(RegenWhole);
	}
}

void UPlayerStaminaComponent::SprintStaminaTick(float DeltaSeconds)
{
	if (!OwnerPlayer) return;

	if (!OwnerPlayer->bIsSprinting) return;

	if (OwnerPlayer->CombatStats.CurrentStamina <= 0)
	{
		OwnerPlayer->EndSprint();
		return;
	}

	const int32 DrainPerSec = OwnerPlayer->StaminaCost_SprintPerSecond;
	if (DrainPerSec <= 0) return;

	OwnerPlayer->SprintDrainAccum += static_cast<float>(DrainPerSec) * DeltaSeconds;

	const int32 DrainWhole = FMath::FloorToInt(OwnerPlayer->SprintDrainAccum);

	if (DrainWhole > 0)
	{
		OwnerPlayer->SprintDrainAccum -= static_cast<float>(DrainWhole);

		ConsumeStamina(DrainWhole);

		if (OwnerPlayer->CombatStats.CurrentStamina <= 0)
		{
			OwnerPlayer->EndSprint();
		}
	}
}

bool UPlayerStaminaComponent::HasFP(int32 Amount) const
{
	if (!OwnerPlayer) return false;

	return OwnerPlayer->CombatStats.CurrentFP >= Amount;
}

bool UPlayerStaminaComponent::TryConsumeFP(int32 Amount)
{
	if (!HasFP(Amount)) return false;

	ConsumeFP(Amount);
	return true;
}

void UPlayerStaminaComponent::ConsumeFP(int32 Amount)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CombatStats.CurrentFP =
		FMath::Clamp(
			OwnerPlayer->CombatStats.CurrentFP - Amount,
			0,
			OwnerPlayer->DerivedStats.MaxFP
		);

	OwnerPlayer->OnFPChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentFP,
		OwnerPlayer->DerivedStats.MaxFP
	);

	UE_LOG(LogTemp, Verbose, TEXT("FP consumed: -%d (Now %d/%d)"),
		Amount,
		OwnerPlayer->CombatStats.CurrentFP,
		OwnerPlayer->DerivedStats.MaxFP
	);
}

