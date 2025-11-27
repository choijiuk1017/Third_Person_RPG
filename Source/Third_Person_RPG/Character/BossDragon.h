// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "BossDragon.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API ABossDragon : public ABossCharacter
{
	GENERATED_BODY()
public:
	//ABossDragon();

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



	void StartBreath();
	void ApplyBreathDamage();
	void EndBreath();


};
