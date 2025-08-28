// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "Third_Person_RPG/Interface/AnimationAttackInterface.h"
#include "Third_Person_RPG/UI/HPBar.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitReact")
	float HitReactDuration = 0.5f;

	UPROPERTY()
	UHPBar* HPBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHPBar> HPBarWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Reward")
	void RegisterAttacker(AActor* Attacker);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> HitReactMontage;


private:
	FTimerHandle HitReactTimerHandle;

	FTimerHandle HideHPBarTimerHandle;

	UFUNCTION()
	void EndHitReact();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward", meta = (AllowPrivateAccess = "true"))
	int32 CurrencyReward = 50;

	// 마지막으로 이 적에게 피해를 준 플레이어
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<APlayerCharacter> LastAttacker;

	// 보상 중복 지급 방지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward", meta = (AllowPrivateAccess = "true"))
	bool bRewardGranted = false;

	void GrantCurrencyToKiller();
};
