// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"

#include "Third_Person_RPG/Data/MMComboActionData.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Third_Person_RPG/Character/TPRPlayerController.h"
#include "Third_Person_RPG/Data/SkillData.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"	
#include "Third_Person_RPG/UI/InventoryUI/InventoryWidget.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Third_Person_RPG/UI/DialogueChoiceWidget.h"
#include "Third_Person_RPG/UI/SavePointUI/SavePointMenu.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Third_Person_RPG/UI/PlayerStatusWidget.h" 
#include "Third_Person_RPG/UI/DeathScreenWidget.h"
#include "Third_Person_RPG/UI/CurrencyWidget.h" 
#include "Third_Person_RPG/UI/CurrentEquipedWidget.h" 
#include "Third_Person_RPG/UI/DialogueWidget.h"
#include "Third_Person_RPG/Actor/NPC.h"
#include "Third_Person_RPG/UI/Tutorial/TutorialWidget.h"
#include "Third_Person_RPG/UI/pauseMenuWidget.h"
#include "Third_Person_RPG/Component/PlayerCombatComponent.h"
#include "Third_Person_RPG/Component/PlayerStaminaComponent.h"
#include "Third_Person_RPG/Component/PlayerUIComponent.h"
#include "Third_Person_RPG/Component/PlayerSavePointComponent.h"
#include "Third_Person_RPG/Component/PlayerInteractionComponent.h"
#include "Third_Person_RPG/Component/PlayerStatComponent.h"


#include "Blueprint/UserWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"  
#include "InputCoreTypes.h" 
#include "GameFramework/PlayerController.h"


#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	Tags.Add(FName("Player"));

	bIsDead = false;
	bIsRoll = false;
	bIsAttacking = false;
	bIsInteracting = false;
	bIsKneeling = false;
	bIsPopupInventory = false;
	bIsSkillActing = false;
	bHasComboInput = false;
	bIsSprinting = false;
	bChoiceUIOpen = false;
	bIsPray = false;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, 90.0f, 0.0f)));

	SpringArmComp->SetupAttachment(GetMesh());

	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	SpringArmComp->bUsePawnControlRotation = true;

	GetCharacterMovement()->bAllowAnyoneToDestroyMe = true;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCharacterMovement()->bIgnoreBaseRotation = true;

	CurrentWeapon = nullptr;
	SkillData = nullptr;

	WeaponComboData = nullptr;

	// 컨트롤러의 Rotation에 영향 X
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 움직임에 따른 회전 On
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.f);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(CHANNEL_ACTION, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	CombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));

	StaminaComponent = CreateDefaultSubobject<UPlayerStaminaComponent>(TEXT("StaminaComponent"));

	UIComponent = CreateDefaultSubobject<UPlayerUIComponent>(TEXT("UIComponent"));

	SavePointComponent = CreateDefaultSubobject<UPlayerSavePointComponent>(TEXT("SavePointComponent"));

	InteractionComponent = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("InteractionComponent"));

	StatComponent = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("StatComponent"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));


	CalculateDerivedStats();
	InitializeCombatStats();


	if (InteractionWidgetClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			InteractionWidgetInstance = CreateWidget<UInteractionWidget>(PC, InteractionWidgetClass);
			if (InteractionWidgetInstance)
			{
				InteractionWidgetInstance->AddToViewport();
				InteractionWidgetInstance->SetIsEnabled(false);
				InteractionWidgetInstance->SetRenderOpacity(0.0f);
				InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (GI)
	{
		const FName TargetID = GI->GetPendingSavePoint();
		if (TargetID != NAME_None)
		{
			const TMap<FName, FSavePointInfo>& SaveMap = GI->GetSavePointMap();
			if (const FSavePointInfo* FoundInfo = SaveMap.Find(TargetID))
			{
				SetActorLocation(FoundInfo->Location + FVector(70.f, 0.f, 50.f));
				UE_LOG(LogTemp, Warning, TEXT("로드 후 세이브포인트로 이동: %s"), *FoundInfo->Location.ToString());
			}

			// 클리어
			const_cast<UTPRGameInstance*>(GI)->ClearPendingSavePoint();

			

		}

		if (InventoryComponent)
		{
			UInventoryItem* EquipTarget = nullptr;
			const FPrimaryAssetId EquippedId = GI->GetCachedEquippedWeaponAssetId();
			for (const FInventoryItemSaveData& SaveData : GI->GetCachedInventory())
			{
				UInventoryItem* AddedItem = InventoryComponent->AddItemByData(
					SaveData.ItemData,
					SaveData.Quantity,
					/*bEquip=*/false
				);

				if (AddedItem)
				{
					AddedItem->EnhanceLevel = SaveData.EnhanceLevel;

					if (EquippedId.IsValid()
						&& SaveData.ItemData
						&& SaveData.ItemData->GetPrimaryAssetId() == EquippedId
						&& Cast<UWeaponItemData>(SaveData.ItemData))
					{
						EquipTarget = AddedItem;
					}
				}
			}

			if (EquipTarget)
			{
				EquipWeapon_Implementation(EquipTarget);
			}
		}

		if (CombatComponent)
		{
			CombatComponent->InitializeCombatComponent(this);
		}

		if (StaminaComponent)
		{
			StaminaComponent->InitializeStaminaComponent(this);
		}

		if (UIComponent)
		{
			UIComponent->InitializeUIComponent(this);
		}

		if (SavePointComponent)
		{
			SavePointComponent->InitializeSavePointComponent(this);
		}

		if (InteractionComponent)
		{
			InteractionComponent->InitializeInteractionComponent(this);
		}

		if (StatComponent)
		{
			StatComponent->InitializeStatComponent(this);
		}

		if (GI->bShouldRespawn)
		{
			const FName LastID = GI->LastRestedSavePointID;
			if (LastID != NAME_None)
			{
				const TMap<FName, FSavePointInfo>& SaveMap = GI->GetSavePointMap();
				if (const FSavePointInfo* FoundInfo = SaveMap.Find(LastID))
				{
					SetActorLocation(FoundInfo->Location + FVector(70.f, 0.f, 50.f));	
				}
			}

			GI->bShouldRespawn = false;
		}

		GI->ApplyLoadedPlayerStatTo(this);
	}	

	if (PlayerStatusWidgetClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PlayerStatusWidgetInstance = CreateWidget<UPlayerStatusWidget>(PC, PlayerStatusWidgetClass);
			if (PlayerStatusWidgetInstance)
			{
				PlayerStatusWidgetInstance->AddToViewport(/*ZOrder=*/10);
				PlayerStatusWidgetInstance->InitWithPlayer(this);

				StatusHUDSavedVisibility = PlayerStatusWidgetInstance->GetVisibility();

				PlayerStatusWidgetInstance->UpdateBarLengths(
					DerivedStats.MaxHP,
					DerivedStats.MaxFP,
					DerivedStats.MaxStamina
				);
			}
		}
	}

	if (CurrencyWidgetClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			CurrencyWidgetInstance = CreateWidget<UCurrencyWidget>(PC, CurrencyWidgetClass);
			if (CurrencyWidgetInstance)
			{
				CurrencyWidgetInstance->AddToViewport(10);
				CurrencyWidgetInstance->BindToPlayer(this);
			}
		}
	}

	if (CurrentEquipedWidgetClass)
	{
		CurrentEquipedWidgetInstance = CreateWidget<UCurrentEquipedWidget>(GetWorld(), CurrentEquipedWidgetClass);
		if (CurrentEquipedWidgetInstance)
		{
			CurrentEquipedWidgetInstance->AddToViewport(/*ZOrder=*/10);
			CurrentEquipedWidgetInstance->UpdateWeaponIcon(CurrentWeaponIcon);

			CurrentEquipedWidgetInstance->UpdatePotionCounts(HPPotionCount, FPPotionCount);

			CurrentEquipedWidgetInstance->ChangePotion(bIsHPPotion, bIsHPPotion ? MaxHPPotionCount : MaxFPPotionCount);
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StaminaComponent)
	{
		StaminaComponent->SprintStaminaTick(DeltaTime);
		StaminaComponent->StaminaRegenTick(DeltaTime);
	}
}

void APlayerCharacter::CalculateDerivedStats()
{
	if (StatComponent)
	{
		StatComponent->CalculateDerivedStats();
	}
}

void APlayerCharacter::InitializeCombatStats()
{
	if (StatComponent)
	{
		StatComponent->InitializeCombatStats();
	}
}

//Combat Section

void APlayerCharacter::ApplyWeaponStats(ATPRWeapon* Weapon)
{
	if (CombatComponent)
	{
		CombatComponent->ApplyWeaponStats(Weapon);
	}
}

void APlayerCharacter::ResetCombatStats()
{
	if (CombatComponent)
	{
		CombatComponent->ResetCombatStats();
	}
}

void APlayerCharacter::BasicAttack()
{
	if (CombatComponent)
	{
		CombatComponent->BasicAttack();
	}
}

void APlayerCharacter::SkillStart()
{
	if (CombatComponent)
	{
		CombatComponent->SkillStart();
	}
}

void APlayerCharacter::ComboStart()
{
	if (CombatComponent)
	{
		CombatComponent->ComboStart();
	}
}

void APlayerCharacter::ComboEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (CombatComponent)
	{
		CombatComponent->ComboEnd(Montage, bInterrupted);
	}
}

void APlayerCharacter::ComboCheck()
{
	if (CombatComponent)
	{
		CombatComponent->ComboCheck();
	}
}

void APlayerCharacter::BaseAttackCheck()
{
	if (CombatComponent)
	{
		CombatComponent->BaseAttackCheck();
	}
}

void APlayerCharacter::EnableWeaponHitBox()
{
	if (CombatComponent)
	{
		CombatComponent->EnableWeaponHitBox();
	}
}

void APlayerCharacter::DisableWeaponHitBox()
{
	if (CombatComponent)
	{
		CombatComponent->DisableWeaponHitBox();
	}
}

void APlayerCharacter::SkillAttackCheck()
{
	if (CombatComponent)
	{
		CombatComponent->SkillAttackCheck();
	}
}

bool APlayerCharacter::CanSetWeapon()
{
	return CombatComponent ? CombatComponent->CanSetWeapon() : false;
}

void APlayerCharacter::EquipWeapon_Implementation(UInventoryItem* WeaponItem)
{
	if (CombatComponent)
	{
		CombatComponent->EquipWeapon(WeaponItem);
	}
}

float APlayerCharacter::GetCurrentWeaponWeight() const
{
	return CombatComponent ? CombatComponent->GetCurrentWeaponWeight() : 0.f;
}

int32 APlayerCharacter::GetEquippedWeaponEnhanceLevel() const
{
	return CombatComponent ? CombatComponent->GetEquippedWeaponEnhanceLevel() : 0;
}


void APlayerCharacter::UnEquipWeapon_Implementation(UInventoryItem* WeaponItem)
{
	if (CombatComponent)
	{
		CombatComponent->UnEquipWeapon(WeaponItem);
	}
}

void APlayerCharacter::BasicMove(const FInputActionValue& Value)
{
	// 입력받은 Value로부터 MovementVector 가져오기
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 컨트롤러의 회전 중 Yaw(Z)를 가져와 저장
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// 회전(Yaw)을 기반으로 전방 및 오른쪽 방향을 받아오기 (X : 전방, Y : 오른쪽)
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Movement에 값 전달 (방향, 이동량)
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void APlayerCharacter::BasicLook(const FInputActionValue& Value)
{
	// 입력받은 Value로부터 LookVector 가져오기
	FVector2D LookVector = Value.Get<FVector2D>();

	// Controller에 값 전달
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}


void APlayerCharacter::BeginSprint()
{
	if (bIsSprinting) return;
	if (bIsDead) return;
	// 시작 즉시 소모가 있다면 체크
	if (StaminaCost_SprintStart > 0)
	{
		if (!TryConsumeStamina(StaminaCost_SprintStart))
		{
			return; // 시작 비용 못 내면 스프린트 불가
		}
	}
	else
	{
		if (!HasStamina(1)) return; // 스태미너 0이면 스프린트 시작 불가
	}

	bIsSprinting = true;
	SprintDrainAccum = 0.f;

	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void APlayerCharacter::EndSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void APlayerCharacter::RollStart()
{
	// 구르기 중이면 리턴
	if (bIsInteracting || bIsRoll || bIsSkillActing) return;
	if (bIsDead) return;
	if (CombatStats.CurrentStamina < StaminaCost_Roll) return;
	ConsumeStamina(StaminaCost_Roll);


	const float Half = DerivedStats.MaxEquipLoad * 0.5f;
	const float W = GetCurrentWeaponWeight();

	// 애님 인스턴스 가져오기
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// Roll Check (구르기 활성화)
		bIsRoll = true;

		if (W <= Half - 5.f)
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(RollMontage, 2.0f);
		}
		else if (W > Half - 5.f && W < Half + 5.f)
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(RollMontage, 1.5f);
		}
		else
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(RollMontage, 1.0f);
		}
	
		// 몽타주 재생 종료 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::RollEnd);

		// RollMontage 종료 시 EndDelegate에 연동된 함수 호출
		AnimInstance->Montage_SetEndDelegate(EndDelegate, RollMontage);

		
	}
}

void APlayerCharacter::RollEnd(class UAnimMontage* Montage, bool IsEnded)
{
	// Roll UnCheck (구르기 비활성화)
	bIsRoll = false;
}


void APlayerCharacter::Interact()
{
	if (InteractionComponent)
	{
		InteractionComponent->Interact();
	}
}

void APlayerCharacter::InteractingSavePoint(UAnimMontage* Montage, bool bInterrupted)
{
	if (SavePointComponent)
	{
		SavePointComponent->InteractingSavePoint(Montage, bInterrupted);
	}
}

void APlayerCharacter::EndInteractSavePoint()
{
	if (SavePointComponent)
	{
		SavePointComponent->EndInteractSavePoint();
	}
}

void APlayerCharacter::OnEquipAnimationEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (InteractionComponent)
	{
		InteractionComponent->OnEquipAnimationEnd(Montage, bInterrupted);
	}
}
void APlayerCharacter::SetOverlappingItem(AItem* Item)
{
	if (InteractionComponent)
	{
		InteractionComponent->SetOverlappingItem(Item);
	}
}

void APlayerCharacter::ReSetOverlappingItem()
{
	if (InteractionComponent)
	{
		InteractionComponent->ReSetOverlappingItem();
	}
}

void APlayerCharacter::SetOverlappingSavePoint(ASavePoint* SavePoint)
{
	if (SavePointComponent)
	{
		SavePointComponent->SetOverlappingSavePoint(SavePoint);
	}
}

void APlayerCharacter::ReSetOverlappingSavePoint()
{
	if (SavePointComponent)
	{
		SavePointComponent->ReSetOverlappingSavePoint();
	}
}




void APlayerCharacter::SetWeapon(ATPRWeapon* NewWeapon)
{
	check(nullptr != NewWeapon && nullptr == CurrentWeapon);

	FName WeaponSocket(TEXT("WeaponSocket"));
	if (NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);

		NewWeapon->SetActorRelativeLocation(NewWeapon->RelativeLocation);
		NewWeapon->SetActorRelativeRotation(NewWeapon->RelativeRotation);
		NewWeapon->SetActorScale3D(NewWeapon->RelativeScale);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;

		SkillData = NewWeapon->SkillData;

		WeaponComboData = NewWeapon->ComboData;

		ApplyWeaponStats(NewWeapon);
	}

	if (NewWeapon->HitBox)
	{
		NewWeapon->DisableHitBox();
	}

	UE_LOG(LogTemp, Warning, TEXT("무기 장착"));
}

void APlayerCharacter::ToggleInventory()
{
	if (UIComponent)
	{
		UIComponent->ToggleInventory();
	}
}

void APlayerCharacter::PopUpInventory()
{
	if (UIComponent)
	{
		UIComponent->PopUpInventory();
	}
}

void APlayerCharacter::CloseInventory()
{
	if (UIComponent)
	{
		UIComponent->CloseInventory();
	}
}

UInventoryComponent* APlayerCharacter::GetInventoryComponent()
{
	return InventoryComponent;
}

void APlayerCharacter::ShowInteractionUI(const FText& InText)
{
	if (UIComponent)
	{
		UIComponent->ShowInteractionUI(InText);
	}
}

void APlayerCharacter::HideInteractionUI()
{
	if (UIComponent)
	{
		UIComponent->HideInteractionUI();
	}
}



void APlayerCharacter::ShowWeaponInfo_Implementation(UWeaponItemData* WeaponData)
{
	if (UIComponent)
	{
		UIComponent->ShowWeaponInfo(WeaponData);
	}
}

void APlayerCharacter::ConsumeStamina(int32 Amount)
{
	if (StaminaComponent)
	{
		StaminaComponent->ConsumeStamina(Amount);
	}
}

void APlayerCharacter::RestoreStaminaTick(int32 AmountPerTick)
{
	if (StaminaComponent)
	{
		StaminaComponent->RestoreStaminaTick(AmountPerTick);
	}
}

int32 APlayerCharacter::GetStaminaRegenPerSecond() const
{
	return StaminaComponent ? StaminaComponent->GetStaminaRegenPerSecond() : 0;
}

void APlayerCharacter::StaminaRegenTick(float DeltaSeconds)
{
	if (StaminaComponent)
	{
		StaminaComponent->StaminaRegenTick(DeltaSeconds);
	}
}

bool APlayerCharacter::HasStamina(int32 Amount) const
{
	return StaminaComponent ? StaminaComponent->HasStamina(Amount) : false;
}

bool APlayerCharacter::TryConsumeStamina(int32 Amount)
{
	return StaminaComponent ? StaminaComponent->TryConsumeStamina(Amount) : false;
}


void APlayerCharacter::SetStatusHUDVisible(bool bVisible)
{
	if (UIComponent)
	{
		UIComponent->SetStatusHUDVisible(bVisible);
	}
}



void APlayerCharacter::SprintStaminaTick(float DeltaSeconds)
{
	if (StaminaComponent)
	{
		StaminaComponent->SprintStaminaTick(DeltaSeconds);
	}
}

bool APlayerCharacter::HasFP(int32 Amount) const
{
	return CombatStats.CurrentFP >= Amount;
}

bool APlayerCharacter::TryConsumeFP(int32 Amount)
{
	if (!HasFP(Amount)) return false;
	ConsumeFP(Amount);
	return true;
}

void APlayerCharacter::ConsumeFP(int32 Amount)
{
	CombatStats.CurrentFP = FMath::Clamp(CombatStats.CurrentFP - Amount, 0, DerivedStats.MaxFP);

	if (PlayerStatusWidgetInstance)
	{
		// 위젯에 FP 반영 (위젯에 UpdateFP가 있다고 가정 — 기존에 MaxFP, Init 및 Stamina/HP 업데이트 패턴과 일치)
		PlayerStatusWidgetInstance->UpdateFP(CombatStats.CurrentFP, DerivedStats.MaxFP);
	}

	UE_LOG(LogTemp, Verbose, TEXT("FP consumed: -%d (Now %d/%d)"),
		Amount, CombatStats.CurrentFP, DerivedStats.MaxFP);
}

int32 APlayerCharacter::GetCurrentSkillFPCost() const
{
	if (CurrentWeapon && CurrentWeapon->ItemData)
	{
		if (const UWeaponItemData* WData = Cast<UWeaponItemData>(CurrentWeapon->ItemData))
		{
			return FMath::Max(0, WData->WeaponStats.FPCost);
		}
	}
	return 0;
}

void APlayerCharacter::AddCurrency(int32 Amount)
{
	if (StatComponent)
	{
		StatComponent->AddCurrency(Amount);
	}
}

bool APlayerCharacter::SpendCurrency(int32 Amount)
{
	return StatComponent ? StatComponent->SpendCurrency(Amount) : false;
}


void APlayerCharacter::RefreshCurrentEquipped_Weapon(UTexture2D* WeaponIconTexture)
{
	if (UIComponent)
	{
		UIComponent->RefreshCurrentEquipped_Weapon(WeaponIconTexture);
	}
}

void APlayerCharacter::RefreshCurrentEquipped_Potion(UTexture2D* PotionIconTexture, int32 NewCount)
{
	if (UIComponent)
	{
		UIComponent->RefreshCurrentEquipped_Potion(PotionIconTexture, NewCount);
	}
}

void APlayerCharacter::DrinkPotion()
{
	if (bIsDead) return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DrinkPotionMontage)
	{
		AnimInstance->Montage_Play(DrinkPotionMontage);
		
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::UsePotion);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, DrinkPotionMontage);

		return; 
	}
}

void APlayerCharacter::ChangePotion()
{
	bIsHPPotion = bIsHPPotion ? 0 : 1;

	if (bIsHPPotion)
	{
		CurrentEquipedWidgetInstance->ChangePotion(bIsHPPotion, HPPotionCount);
	}
	else
	{
		CurrentEquipedWidgetInstance->ChangePotion(bIsHPPotion, FPPotionCount);
	}
	
}

void APlayerCharacter::UsePotion(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != DrinkPotionMontage)
		return;

	if (!bInterrupted)
	{
		if (bIsHPPotion)
		{
			if (HPPotionCount > 0)
			{
				HPPotionCount -= 1;

				if (CurrentEquipedWidgetInstance)
					CurrentEquipedWidgetInstance->UpdatePotionCounts(HPPotionCount, FPPotionCount);

				CombatStats.CurrentHP = FMath::Min(CombatStats.CurrentHP + 500, DerivedStats.MaxHP);
				OnHPChanged.Broadcast(CombatStats.CurrentHP, DerivedStats.MaxHP);
			}
		}
		else
		{
			if (FPPotionCount > 0)
			{
				FPPotionCount -= 1;
				if (CurrentEquipedWidgetInstance)
					CurrentEquipedWidgetInstance->UpdatePotionCounts(HPPotionCount, FPPotionCount);

				CombatStats.CurrentFP = FMath::Min(CombatStats.CurrentFP + 100, DerivedStats.MaxFP);
				OnFPChanged.Broadcast(CombatStats.CurrentFP, DerivedStats.MaxFP);
			}
		}

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void APlayerCharacter::RecalculateStatsAfterLevelUp(bool bRefillHPFPStamina)
{
	if (StatComponent)
	{
		StatComponent->RecalculateStatsAfterLevelUp(bRefillHPFPStamina);
	}
}
void APlayerCharacter::FillSaveData(FPlayerStatSaveData& OutSaveData) const
{
	if (StatComponent)
	{
		StatComponent->FillSaveData(OutSaveData);
	}
}

void APlayerCharacter::ApplySaveData(const FPlayerStatSaveData& InSaveData)
{
	if (StatComponent)
	{
		StatComponent->ApplySaveData(InSaveData);
	}
}

void APlayerCharacter::RespawnPlayer()
{
	UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (GI)
	{
		GI->bShouldRespawn = true;
	}

	UGameplayStatics::OpenLevel(GetWorld(), *CurrentMapName);
}

void APlayerCharacter::StartRolling()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	if (!Capsule) return;

	Capsule->SetCollisionResponseToChannel(CHANNEL_ACTION, ECR_Ignore);

}

void APlayerCharacter::EndRolling()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (!Capsule) return;

	Capsule->SetCollisionResponseToChannel(CHANNEL_ACTION, ECR_Block);
}

void APlayerCharacter::SetCurrentNPC(ANPC* InNPC)
{
	if (InteractionComponent)
	{
		InteractionComponent->SetCurrentNPC(InNPC);
	}
}

void APlayerCharacter::OnAdvanceDialogue()
{
	if (bChoiceUIOpen) return;

	if (CurrentNPC)
	{
		CurrentNPC->AdvanceTalk(this);
	}
}

void APlayerCharacter::OpenDialogueUI()
{
	if (!DialogueWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!DialogueWidgetInstance)
	{
		DialogueWidgetInstance = CreateWidget<UDialogueWidget>(PC, DialogueWidgetClass);
	}

	if (DialogueWidgetInstance && !DialogueWidgetInstance->IsInViewport())
	{
		DialogueWidgetInstance->AddToViewport(100);

		FInputModeGameAndUI Mode;
		Mode.SetWidgetToFocus(DialogueWidgetInstance->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(true);

		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = false;
	}
}

void APlayerCharacter::CloseDialogueUI()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = false;
	}

	if (DialogueWidgetInstance && DialogueWidgetInstance->IsInViewport())
	{
		DialogueWidgetInstance->RemoveFromParent();
	}
}

void APlayerCharacter::SetDialogueLine(const FText& InText)
{
	if (DialogueWidgetInstance)
	{
		DialogueWidgetInstance->SetDialogueText(InText);
	}
}

void APlayerCharacter::OpenChoiceUI(const FText& Question, const FText& Yes, const FText& No)
{
	if (!DialogueChoiceWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!DialogueChoiceWidgetInstance)
	{
		DialogueChoiceWidgetInstance = CreateWidget<UDialogueChoiceWidget>(PC, DialogueChoiceWidgetClass);
		if (!DialogueChoiceWidgetInstance) return;

		DialogueChoiceWidgetInstance->OnChoiceConfirmed.AddUObject(this, &APlayerCharacter::HandleChoiceConfirmed);
	}

	DialogueChoiceWidgetInstance->SetTexts(Question, Yes, No);
	DialogueChoiceWidgetInstance->SetSelectedIndex(0);

	if (!DialogueChoiceWidgetInstance->IsInViewport())
	{
		DialogueChoiceWidgetInstance->AddToViewport(110);
	}

	bChoiceUIOpen = true;

	FInputModeGameAndUI Mode;
	Mode.SetWidgetToFocus(DialogueChoiceWidgetInstance->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(true);

	PC->SetInputMode(Mode);
	PC->bShowMouseCursor = false;


	DialogueChoiceWidgetInstance->SetKeyboardFocus();
}

void APlayerCharacter::CloseChoiceUI()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	bChoiceUIOpen = false;

	if (DialogueChoiceWidgetInstance && DialogueChoiceWidgetInstance->IsInViewport())
	{
		DialogueChoiceWidgetInstance->RemoveFromParent();
	}
	if (PC)
	{
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);

	}
}

void APlayerCharacter::HandleChoiceConfirmed(bool bYes)
{
	CloseChoiceUI();

	if (CurrentNPC)
	{
		CurrentNPC->ConfirmChoice(this, bYes);
	}
}

void APlayerCharacter::ApplyPrayerBuff()
{
	CombatStats.CurrentHP += 500;
	CombatStats.CurrentFP += 200;

	CombatStats.CurrentStamina += 500;

	CombatStats.AttackPower += 50;

	CombatStats.Defense += 10;

	CombatStats.Poise += 10;
}

void APlayerCharacter::RemovePrayerBuff()
{
	CombatStats.CurrentHP -= 500;
	CombatStats.CurrentFP -= 200;

	CombatStats.CurrentStamina -= 500;

	CombatStats.AttackPower -= 50;

	CombatStats.Defense -= 10;

	CombatStats.Poise -= 10;
}

void APlayerCharacter::Prayer()
{
	if (bIsPray)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PrayerTimerHandle);
			World->GetTimerManager().SetTimer(PrayerTimerHandle, this, &APlayerCharacter::EndPrayer, 300.0f, false);
		}
		return;
	}

	bIsPray = true;

	ApplyPrayerBuff();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrayerTimerHandle);
		World->GetTimerManager().SetTimer(PrayerTimerHandle, this, &APlayerCharacter::EndPrayer, 300.0f, false);
	}
}

void APlayerCharacter::EndPrayer()
{
	if (!bIsPray)
	{
		return;
	}

	RemovePrayerBuff();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PrayerTimerHandle);
	}
}

void APlayerCharacter::OpenMovementTutorial()
{
	if (!TutorialWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!TutorialWidgetInstance)
	{
		TutorialWidgetInstance = CreateWidget<UTutorialWidget>(PC, TutorialWidgetClass);
		if (!TutorialWidgetInstance) return;
	}

	if (!TutorialWidgetInstance->IsInViewport())
	{
		TutorialWidgetInstance->AddToViewport(1000000);
	}
}

void APlayerCharacter::ForceStopActionsForCutscene()
{
	EndSprint();            
	
	bIsAttacking = false;
	bIsRoll = false;
	bIsSkillActing = false;

	GetCharacterMovement()->StopMovementImmediately();
}

void APlayerCharacter::TogglePauseMenu()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UpauseMenuWidget>(PC, PauseMenuWidgetClass);
	}

	if (!PauseMenuWidget) return;

	if (PauseMenuWidget->IsInViewport())
	{
		PauseMenuWidget->Close();
	}
	else
	{
		PauseMenuWidget->Open(PC);
	}
}

void APlayerCharacter::TakeDamage(int32 DamageAmount)
{
	if (StatComponent)
	{
		StatComponent->TakeDamage(DamageAmount);
	}
}
