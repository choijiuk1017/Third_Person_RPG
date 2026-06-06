// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatComponent.generated.h"

class APlayerCharacter;
class ATPRWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerStatComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;

public:
	void InitializeStatComponent(APlayerCharacter* InOwner);

	void CalculateDerivedStats();
	void InitializeCombatStats();
	void RecalculateStatsAfterLevelUp(bool bRefillHPFPStamina = false);

	void TakeDamage(int32 DamageAmount);

	void AddCurrency(int32 Amount);
	bool SpendCurrency(int32 Amount);

	void FillSaveData(struct FPlayerStatSaveData& OutSaveData) const;
	void ApplySaveData(const struct FPlayerStatSaveData& InSaveData);
};
