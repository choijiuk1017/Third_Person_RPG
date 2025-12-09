// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "Third_Person_RPG/Interface/AnimationAttackInterface.h"
#include "Third_Person_RPG/UI/HPBar.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Data/SkillData.h"

#include "Delegates/DelegateCombinations.h"

#include "BossCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDiedDelegate);


USTRUCT(BlueprintType)
struct FBossStats
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
class THIRD_PERSON_RPG_API ABossCharacter : public ACharacter, public IAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossCharacter();

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FOnBossDiedDelegate OnBossDied;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BaseAttackCheck() override;
	virtual void SkillAttackCheck() override {};
	virtual void SkillAttackCheckByIndex(int32 Index) override;
	virtual void EnableWeaponHitBox() override {};
	virtual void DisableWeaponHitBox() override {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

	void TakeDamage(int32 DamageAmount);

	void PlayPatternMontage(int32 Index);
	int32 GetCurrentPatternIndex() const { return CurrentPatternIndex; }


	UPROPERTY(EditAnywhere, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, Category = "Pattern")
	TArray<USkillData*> PatternDatas;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Attack")
	int32 CurrentAttackStep = 0;


	UFUNCTION(BlueprintCallable, Category = "Combat|Attack")
	void PlayAttackMontageByIndex(int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FBossStats BossStats;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsDead;

	UPROPERTY()
	UHPBar* HPBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHPBar> HPBarWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Reward")
	void RegisterAttacker(AActor* Attacker);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitReact")
	float HitReactDuration = 0.5f;

	void PlayHitReactMontage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BossType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UStaticMeshComponent* WeaponMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> HitReactMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 MaxGroggy = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 CurrentGroggy = 0;

	FTimerHandle HitReactTimerHandle;

	FTimerHandle HideHPBarTimerHandle;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward", meta = (AllowPrivateAccess = "true"))
	int32 CurrencyReward = 50;

	// 마지막으로 이 적에게 피해를 준 플레이어
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<APlayerCharacter> LastAttacker;

	// 보상 중복 지급 방지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward", meta = (AllowPrivateAccess = "true"))
	bool bRewardGranted = false;

	void GrantCurrencyToKiller();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void EndHitReact();

	void SpawnSkillEffectByData(const USkillData* Data);

	void PatternEnd(UAnimMontage* Montage, bool IsEnded);

	int32 CurrentPatternIndex = -1;

	APlayerCharacter* PlayerRef;
};
