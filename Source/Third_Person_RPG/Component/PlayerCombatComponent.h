// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "PlayerCombatComponent.generated.h"

class APlayerCharacter;
class ATPRWeapon;
class UInventoryItem;
class UWeaponItemData;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCombatComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;

public:
	void InitializeCombatComponent(APlayerCharacter* InOwner);

	void BasicAttack();
	void ComboStart();
	void ComboCheck();
	void ComboEnd(UAnimMontage* Montage, bool bInterrupted);
	void SetComboTimer();

	void BaseAttackCheck();

	void SkillStart();
	void SkillAttackCheck();
	void SpawnSkillEffect();

	void EnableWeaponHitBox();
	void DisableWeaponHitBox();

	void ApplyWeaponStats(ATPRWeapon* Weapon);
	void ResetCombatStats();

	void EquipWeapon(UInventoryItem* WeaponItem);
	void UnEquipWeapon(UInventoryItem* WeaponItem);
	void SetWeapon(ATPRWeapon* NewWeapon);

	bool CanSetWeapon() const;
	float ConvertScalingToMultiplier(const FString& Scaling) const;

	int32 GetEquippedWeaponEnhanceLevel() const;
	float GetCurrentWeaponWeight() const;

protected:
	UPROPERTY()
	TArray<UParticleSystemComponent*> SkillEffectPool;

	UPROPERTY(EditAnywhere, Category = "Skill Effect Pool")
	int32 SkillEffectPoolSize = 10;

	UPROPERTY(EditAnywhere, Category = "Skill Effect Pool")
	float SkillEffectLifeTime = 2.0f;

	void InitializeSkillEffectPool();

	UParticleSystemComponent* GetAvailableSkillEffect();

	void ReturnSkillEffectToPool(UParticleSystemComponent* EffectComponent);
		
};
