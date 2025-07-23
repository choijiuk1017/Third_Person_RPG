// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Third_Person_RPG/Interface/AnimationAttackInterface.h"
#include "EnemyCharacter.generated.h"

USTRUCT(BlueprintType)
struct FEnemyStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Defense;
};

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

	void TakeDamage(int32 DamageAmount);

	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<class UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FEnemyStats EnemyStats;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsDead;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> HitReactMontage;


public:	


};
