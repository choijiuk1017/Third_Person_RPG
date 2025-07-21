// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Third_Person_RPG/Interface/AnimationAttackInterface.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class THIRD_PERSON_RPG_API AEnemyCharacter : public ACharacter, public IAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BaseAttackCheck() override;
	virtual void SkillAttackCheck() override {};
	virtual void EnableWeaponHitBox() override {};
	virtual void DisableWeaponHitBox() override {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

	void PlayHitReactMontage();

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackMontage;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* HitReactMontage;


public:	


};
