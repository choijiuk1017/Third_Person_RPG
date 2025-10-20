// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Third_Person_RPG/Interface/AnimationAttackInterface.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/Item/Item.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"

#include "Third_Person_RPG/UI/SavePointUI/SavePointMenu.h"
#include "Blueprint/UserWidget.h"
#include "Third_Person_RPG/UI/PlayerStatusWidget.h"
#include "Third_Person_RPG/UI/CurrencyWidget.h"
#include "Third_Person_RPG/UI/CurrentEquipedWidget.h"
#include "Delegates/DelegateCombinations.h"

#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, int32, int32); //현재, 최대
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFPChanged, int32, int32); //현재, 최대
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, int32, int32); //현재, 최대
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrencyChanged, int32, NewCurrency);

USTRUCT(BlueprintType)
struct FCharacterAttributes
{
	GENERATED_BODY()

	// 생명력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Vigor = 10;

	// 정신력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Mind = 10;

	// 지구력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Endurance = 10;

	// 근력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Strength = 10;

	// 기량
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Dexterity = 10;

	// 지력
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Intelligence = 10;

	// 신앙
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Faith = 10;

	// 신비
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Arcane = 10;
};

USTRUCT(BlueprintType)
struct FDerivedStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHP = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxFP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxEquipLoad = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Poise = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Discovery = 100.f; // 발견력
};

USTRUCT(BlueprintType)
struct FCombatStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentFP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Poise;
};

UCLASS()
class THIRD_PERSON_RPG_API APlayerCharacter : public ACharacter, public IAnimationAttackInterface, public IInventoryInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	FOnHPChanged OnHPChanged;
	FOnFPChanged OnFPChanged;
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCurrencyChanged OnCurrencyChanged;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UInventoryComponent* GetInventoryComponent() override;

	virtual void EquipWeapon_Implementation(UInventoryItem* WeaponItem) override;

	virtual void UnEquipWeapon_Implementation(UInventoryItem* WeaponItem) override;

	virtual void ShowWeaponInfo_Implementation(UWeaponItemData* WeaponData) override;

	virtual void SkillAttackCheckByIndex(int32 Index) override {};

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsKneeling;


	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class ATPRWeapon* CurrentWeapon;

	bool CanSetWeapon();
	void SetWeapon(class ATPRWeapon* NewWeapon);


	void SetOverlappingItem(AItem* Item);

	void ReSetOverlappingItem();

	void SetOverlappingSavePoint(ASavePoint* SavePoint);

	void EndInteractSavePoint();

	void ReSetOverlappingSavePoint();

	void TakeDamage(int32 DamageAmount);

	void ShowInteractionUI(const FText& InText);

	void HideInteractionUI();

	void SetStatusHUDVisible(bool bVisible);

	float GetCurrentWeaponWeight() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInventoryComponent> InventoryComponent;

	UPROPERTY()
	USavePointMenu* SavePointMenuInstance;

	// 캐릭터 기본 능력치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FCharacterAttributes CharacterAttributes;

	// 파생 능력치 (최대 HP/FP 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FDerivedStats DerivedStats;

	// 전투 중 수치 (현재 HP/FP 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FCombatStats CombatStats;

	// 캐릭터 레벨 (별도로 관리)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency")
	int32 Currency = 0;

	void AddCurrency(int32 Amount);
	bool SpendCurrency(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Currency")
	int32 GetCurrency() const { return Currency; }

	UFUNCTION(BlueprintCallable, Category = "UI|CurrentEquipped")
	void RefreshCurrentEquipped_Weapon(UTexture2D* WeaponIconTexture);

	UFUNCTION(BlueprintCallable, Category = "UI|CurrentEquipped")
	void RefreshCurrentEquipped_Potion(UTexture2D* PotionIconTexture, int32 NewCount);

	UFUNCTION(BlueprintCallable)
	void RecalculateStatsAfterLevelUp(bool bRefillHPFPStamina = false);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	int32 MaxPotionCount = 7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	int32 MaxHPPotionCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	int32 MaxFPPotionCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	int32 HPPotionCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	int32 FPPotionCount = 2;

	void FillSaveData(struct FPlayerStatSaveData& OutSaveData) const;

	void ApplySaveData(const struct FPlayerStatSaveData& InSaveData);

	void BasicAttack();

	//구르기 시작 함수
	void RollStart();

	void BeginSprint();

	void EndSprint();

	void Interact(); // 상호작용 키로 호출할 함수

	void ToggleInventory();

	void DrinkPotion();

	void ChangePotion();

	//스킬 함수
	void SkillStart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Component Section
	//스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;

	//카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;

	UPROPERTY()
	class AItem* OverlappingItem;


	UPROPERTY()
	class ASavePoint* OverlappingSavePoint;

	//Montage Section
	//구르기 애니메이션 몽타주
	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> PickUpMontage;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> KneelingDownMontage;

	UPROPERTY(EditAnywhere, Category = Montage, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> KneelingIdleMontage;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	TObjectPtr<class UAnimMontage> DrinkPotionMontage;
	

	//Data Section
	//콤보 공격 데이터
	UPROPERTY(EditAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> BasicComboData;

	UPROPERTY(VisibleAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> WeaponComboData;

	//스킬 공격 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkillData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkillData> SkillData;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;
	
	UInventoryWidget* InventoryWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class USavePointMenu> SavePointMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UInteractionWidget* InteractionWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY()
	UPlayerStatusWidget* PlayerStatusWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI|CurrentEquipped")
	TSubclassOf<UCurrentEquipedWidget> CurrentEquipedWidgetClass;

	UPROPERTY()
	TObjectPtr<UCurrentEquipedWidget> CurrentEquipedWidgetInstance;


	UPROPERTY(EditDefaultsOnly, Category = "Consumable")
	TObjectPtr<UTexture2D> DefaultPotionIcon; // 에스트병 아이콘 등

	// (선택) 현재 장착 무기 아이콘 캐시
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	TObjectPtr<UTexture2D> CurrentWeaponIcon = nullptr;



	UPROPERTY(VisibleAnywhere, Category = "UI")
	bool bPausedBySavePoint = false;


	UFUNCTION()
	void SprintStaminaTick(float DeltaSeconds);

	//Function Section
	//기본 이동 함수
	void BasicMove(const FInputActionValue& Value);
	//카메라 시점 함수
	void BasicLook(const FInputActionValue& Value);


	
	//구르기 종료 함수
	void RollEnd(class UAnimMontage* Montage, bool IsEnded);
	
	//콤보 시작 함수
	void ComboStart();
	//콤보 종료 함수
	void ComboEnd(class UAnimMontage* Montage, bool IsEnded);
	//콤보가 이어지는 지 확인하는 함수
	void ComboCheck();
	//콤보 체크 호출 시간 설정 함수
	void SetComboTimer();

	//스킬 이펙트 소환 함수
	void SpawnSkillEffect();


	//공격 체크 함수, 인터페이스에서 상속 받음
	virtual void BaseAttackCheck() override;

	virtual void EnableWeaponHitBox() override;

	virtual void DisableWeaponHitBox() override;


	//스킬 공격 함수, 인터페이스에서 상속 받음
	virtual void SkillAttackCheck() override;



	void OnEquipAnimationEnd(UAnimMontage* Montage, bool bInterrupted);

	void InteractingSavePoint(UAnimMontage* Montage, bool bInterrupted);

	void PopUpInventory();

	void CloseInventory();

	void CalculateDerivedStats();
	void InitializeCombatStats();

	float ConvertScalingToMultiplier(const FString& Scaling);

	void ApplyWeaponStats(class ATPRWeapon* Weapon);

	void ResetCombatStats();

	void ConsumeStamina(int32 Amount);

	void RestoreStaminaTick(int32 AmountPerTick);

	void StaminaRegenTick(float DeltaSeconds);


	int32 GetStaminaRegenPerSecond() const;
	
	//Variable Section
	//구르기 확인 변수
	uint8 bIsRoll : 1;
	//콤보에 사용될 타이머 변수
	FTimerHandle ComboTimerHandle;
	//현재 콤보 진행 수
	int32 CurrentComboCount;
	//콤보 입력 판별
	uint8 bHasComboInput : 1;
	//공격 중 구르기, 구르기 중 공격 등 다른 모션을 막기 위한 변수
	uint8 bIsAttacking : 1;

	uint8 bIsSkillActing : 1;

	uint8 bIsInteracting : 1;

	uint8 bIsPopupInventory : 1;

	uint8 bIsHPPotion : 1 = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaCost_Roll = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaCost_Attack = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaCost_AttackPerHit = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaCost_Skill = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaRegen_Light = 30;   // (무게 <= MaxEquipLoad*0.5 - 5)

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaRegen_Medium = 20;  // (무게가 절반 ±5 이내)

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaRegen_Heavy = 15;   // (무게 >= MaxEquipLoad*0.5 + 5)

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	bool bRequireStaminaForComboContinue = true;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	bool bEnableFrameStaminaRegen = true;

	float StaminaRegenAccum = 0.f;


	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool HasStamina(int32 Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool TryConsumeStamina(int32 Amount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	int32 StaminaCost_SprintPerSecond = 15;   

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	int32 StaminaCost_SprintStart = 0;    

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	bool bBlockRegenWhileSprinting = true;      

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	bool bIsSprinting = false;

	UPROPERTY(Transient)
	float SprintDrainAccum = 0.f;

	UPROPERTY()
	ESlateVisibility StatusHUDSavedVisibility = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Cost")
	bool bSkillConsumesFP = true;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCurrencyWidget> CurrencyWidgetClass;

	UPROPERTY()
	UCurrencyWidget* CurrencyWidgetInstance = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Skill|Cost")
	int32 GetCurrentSkillFPCost() const;

	UFUNCTION(BlueprintCallable, Category = "Status|FP")
	bool HasFP(int32 Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Status|FP")
	bool TryConsumeFP(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Status|FP")
	void ConsumeFP(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Potion")
	void UsePotion(UAnimMontage* Montage, bool bInterrupted);
};
