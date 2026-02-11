// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "BossDragon.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API ABossDragon : public ABossCharacter
{
	GENERATED_BODY()
public:
	ABossDragon();

	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* BreathEffect;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> GroundBreathMontage;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> FlyBreathMontage;

	FTimerHandle BreathTimerHandle;

	UPROPERTY()
	UParticleSystemComponent* BreathPSC;

	TMap<APlayerCharacter*, float> DamageCooldownMap;

	UPROPERTY(EditAnywhere)
	float FlyHeight = 16000.f;

	UPROPERTY(EditAnywhere)
	float FlyUpSpeed = 1600.f;

	UPROPERTY(EditAnywhere)
	float FallSpeed = 2000.f;

	UPROPERTY(EditAnywhere)
	float LandAttackRadius = 300.f;

	UPROPERTY(EditAnywhere)
	float LandAttackDamage = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFalling = false;


	UPROPERTY(EditAnywhere)
	UParticleSystem* LandAttackEffect;

	UPROPERTY(EditAnywhere)
	float BreathDamageCooldown = 0.2f;

	bool bHasTriggeredFlyPhase = false;

	FTimerHandle FlyFallTimerHandle;

	UPROPERTY()
	FVector TargetFallLocation;

	void StartBreath();
	void ApplyBreathDamage();
	void EndBreath();

	void StartFlyPhase();
	void StartFalling();
	void FlyTick(float DeltaSeconds);
	void OnLand(const FVector& LandPos);

	UPROPERTY(EditAnywhere, Category = "Audio|FlyPhase")
	USoundBase* LandImpactSFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio|FlyPhase", meta = (ClampMin = "0.0"))
	float LandImpactVolume = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Audio|FlyPhase", meta = (ClampMin = "0.1"))
	float LandImpactPitch = 1.0f;

	// 혹시 같은 프레임에 OnLand가 중복 호출될 가능성 방지
	bool bPlayedLandImpactSFX = false;

};
