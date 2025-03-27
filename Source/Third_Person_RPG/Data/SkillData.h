// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillData.generated.h"


UENUM(BlueprintType)
enum class ESkillEffectSpawnType : uint8
{
	Self		UMETA(DisplayName = "Self"),
	Forward		UMETA(DisplayName = "Forward"),
	Ground		UMETA(DisplayName = "Ground"),
	Custom		UMETA(DisplayName = "Custom")
};
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API USkillData : public UDataAsset
{
	GENERATED_BODY()

public:
	USkillData();

public:
	UPROPERTY(EditAnywhere, Category = SkillRange)
	float SkillRange;

	UPROPERTY(EditAnywhere, Category = SkillRange)
	float SkillRadius;

	UPROPERTY(EditAnywhere, Category = SkillDamage)
	float SkillDamage;

	UPROPERTY(EditAnywhere, Category = SkillPlayeRate)
	float SkillPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	ESkillEffectSpawnType SpawnType = ESkillEffectSpawnType::Forward;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> SkillMontage;

	UPROPERTY(EditAnywhere, Category = Effects, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UParticleSystem> SkillEffect;

};
