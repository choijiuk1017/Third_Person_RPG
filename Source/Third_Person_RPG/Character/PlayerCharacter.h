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

class UPlayerStaminaComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, int32, int32); 
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFPChanged, int32, int32); 
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, int32, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrencyChanged, int32, NewCurrency);

USTRUCT(BlueprintType)
struct FCharacterAttributes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Vigor = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Mind = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Endurance = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Strength = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Dexterity = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Intelligence = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Faith = 10;

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
	float Discovery = 100.f; 
};

USTRUCT(BlueprintType)
struct FCombatStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentFP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentStamina = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Poise = 0;
};

class ANPC;
class UDialogueWidget;
class ATPRWeapon;
class UDialogueChoiceWidget;
class UTutorialWidget;
class UpauseMenuWidget;

UCLASS()
class THIRD_PERSON_RPG_API APlayerCharacter : public ACharacter, public IAnimationAttackInterface, public IInventoryInterface
{
	GENERATED_BODY()

	friend class UPlayerCombatComponent;
	friend class UPlayerStaminaComponent;

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

	void ForceStopActionsForCutscene();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	ATPRWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintPure, Category = "Components")
	UPlayerCombatComponent* GetCombatComponent() const { return CombatComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInventoryComponent> InventoryComponent;

	UPROPERTY()
	USavePointMenu* SavePointMenuInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FCharacterAttributes CharacterAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FDerivedStats DerivedStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	FCombatStats CombatStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency")
	int32 Currency = 0;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsDead;

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

	UPROPERTY(SaveGame)
	int32 SpentCurrencyOnStats = 0;

	void FillSaveData(struct FPlayerStatSaveData& OutSaveData) const;

	void ApplySaveData(const struct FPlayerStatSaveData& InSaveData);

	void BasicAttack();

	void RollStart();

	void BeginSprint();

	void EndSprint();

	void Interact(); 

	void ToggleInventory();

	void DrinkPotion();

	void ChangePotion();

	void SkillStart();


	void SetCurrentNPC(ANPC* InNPC);
	void OpenDialogueUI();
	void CloseDialogueUI();
	void SetDialogueLine(const FText& InText);

	void OnAdvanceDialogue();
	void ApplyWeaponStats(class ATPRWeapon* Weapon);

	void OpenChoiceUI(const FText& Question, const FText& Yes, const FText& No);
	void CloseChoiceUI();

	void Prayer();
	void EndPrayer();

	FTimerHandle PrayerTimerHandle;

	void OpenMovementTutorial();


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UpauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY()
	UpauseMenuWidget* PauseMenuWidget = nullptr;

	void TogglePauseMenu();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Component Section
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;

	UPROPERTY()
	class AItem* OverlappingItem;

	UPROPERTY()
	class ASavePoint* OverlappingSavePoint;

	UPROPERTY()
	ANPC* CurrentNPC = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	UPROPERTY()
	UDialogueWidget* DialogueWidgetInstance = nullptr;


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UTutorialWidget> TutorialWidgetClass;

	UPROPERTY()
	UTutorialWidget* TutorialWidgetInstance = nullptr;


	//Montage Section
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
	UPROPERTY(EditAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> BasicComboData;

	UPROPERTY(VisibleAnywhere, Category = ComboData, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMMComboActionData> WeaponComboData;

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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DeathScreenWidgetClass;

	UPROPERTY()
	UPlayerStatusWidget* PlayerStatusWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI|CurrentEquipped")
	TSubclassOf<UCurrentEquipedWidget> CurrentEquipedWidgetClass;

	UPROPERTY()
	TObjectPtr<UCurrentEquipedWidget> CurrentEquipedWidgetInstance;


	UPROPERTY(EditDefaultsOnly, Category = "Consumable")
	TObjectPtr<UTexture2D> DefaultPotionIcon; 

	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	TObjectPtr<UTexture2D> CurrentWeaponIcon = nullptr;



	UPROPERTY(VisibleAnywhere, Category = "UI")
	bool bPausedBySavePoint = false;

	void HandleChoiceConfirmed(bool bYes);

	UFUNCTION()
	void SprintStaminaTick(float DeltaSeconds);

	//Function Section
	void BasicMove(const FInputActionValue& Value);
	void BasicLook(const FInputActionValue& Value);

	void RollEnd(class UAnimMontage* Montage, bool IsEnded);
	
	void ComboStart();

	void ComboEnd(class UAnimMontage* Montage, bool IsEnded);

	void ComboCheck();

	void SpawnSkillEffect();


	virtual void BaseAttackCheck() override;

	virtual void EnableWeaponHitBox() override;

	virtual void DisableWeaponHitBox() override;


	virtual void SkillAttackCheck() override;

	virtual void StartRolling() override;
	virtual void EndRolling() override;

	

	void OnEquipAnimationEnd(UAnimMontage* Montage, bool bInterrupted);

	void InteractingSavePoint(UAnimMontage* Montage, bool bInterrupted);

	void PopUpInventory();

	void CloseInventory();

	void CalculateDerivedStats();
	void InitializeCombatStats();

	float ConvertScalingToMultiplier(const FString& Scaling);

	void ResetCombatStats();

	void ConsumeStamina(int32 Amount);

	void RestoreStaminaTick(int32 AmountPerTick);

	void StaminaRegenTick(float DeltaSeconds);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDialogueChoiceWidget> DialogueChoiceWidgetClass;

	UPROPERTY()
	UDialogueChoiceWidget* DialogueChoiceWidgetInstance = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	bool bChoiceUIOpen = false;

	UFUNCTION()
	void RespawnPlayer();

	FTimerHandle RespawnTimerHandle;

	int32 GetStaminaRegenPerSecond() const;

	void ApplyPrayerBuff();
	void RemovePrayerBuff();
	

	uint8 bIsPray : 1;


	//Variable Section
	uint8 bIsRoll : 1;

	FTimerHandle ComboTimerHandle;

	int32 CurrentComboCount;

	uint8 bHasComboInput : 1;

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
	int32 StaminaRegen_Light = 30;  

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaRegen_Medium = 20; 

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	int32 StaminaRegen_Heavy = 15; 

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

	UPROPERTY(EditDefaultsOnly, Category = "Enhance")
	float EnhanceDamageRatePerLevel = 0.05f; 

	int32 GetEquippedWeaponEnhanceLevel() const;


	bool bPausedByInventory = false;

	float SavedGlobalTimeDilation = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPlayerCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerStaminaComponent> StaminaComponent;

};
