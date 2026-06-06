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
	DerivedStats.MaxHP = 300 + (CharacterAttributes.Vigor * 50);
	DerivedStats.MaxFP = 50 + (CharacterAttributes.Mind * 10);
	DerivedStats.MaxStamina = 80 + (CharacterAttributes.Endurance * 10);
	DerivedStats.MaxEquipLoad = 30.f + (CharacterAttributes.Endurance * 1.5f);
	DerivedStats.Poise = CharacterAttributes.Endurance * 1.2f;
	DerivedStats.Discovery = CharacterAttributes.Arcane * 1.0f;
}

void APlayerCharacter::InitializeCombatStats()
{
	CombatStats.CurrentHP = DerivedStats.MaxHP;
	CombatStats.CurrentFP = DerivedStats.MaxFP;
	CombatStats.CurrentStamina = DerivedStats.MaxStamina;

	// 무기 장착 전 기본 공격력/방어력
	CombatStats.AttackPower = 50 + (CharacterAttributes.Strength * 2);  // 베이스 값
	CombatStats.Defense = 10 + FMath::RoundToInt(CharacterAttributes.Strength * 1.5f);
	CombatStats.Poise = DerivedStats.Poise;
}

float APlayerCharacter::ConvertScalingToMultiplier(const FString& Scaling)
{
	if (Scaling == "S") return 1.0f;
	if (Scaling == "A") return 0.8f;
	if (Scaling == "B") return 0.6f;
	if (Scaling == "C") return 0.4f;
	if (Scaling == "D") return 0.2f;
	if (Scaling == "E") return 0.1f;
	return 0.0f; // 없음
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
	if (bIsRoll || bIsSkillActing || bIsAttacking || bIsInteracting) return;
	if (bIsDead) return;
	UE_LOG(LogTemp, Warning, TEXT("상호작용 키 입력됨"));

	UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>();


	if (OverlappingItem && OverlappingItem->ItemData)
	{
		FPrimaryAssetId AssetId = OverlappingItem->ItemData->GetPrimaryAssetId();

		if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			GI->RegisterCollectedItemAsset(AssetId);
		}

		//무기를 장착할 수 있을 경우에만 장착 애니메이션
		if (OverlappingItem->WeaponClass && CanSetWeapon())
		{
			bIsInteracting = true;

			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance && PickUpMontage)
			{
				AnimInstance->Montage_Play(PickUpMontage, 1.7f);
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &APlayerCharacter::OnEquipAnimationEnd);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, PickUpMontage);
				return; // 애니메이션 후 처리 예정
			}
		}
		else
		{
			if (Inventory)
			{
				int32 OutQuantity = 1;
				Inventory->AddItemByData(OverlappingItem->ItemData, OutQuantity);
			}

			OverlappingItem->Destroy();
			OverlappingItem = nullptr;
			bIsInteracting = false;
		}
	}

	if (OverlappingSavePoint)
	{
		bIsInteracting = true;

		if (CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			CurrentWeapon->SetActorEnableCollision(false);
		}

		InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		SetStatusHUDVisible(false);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && KneelingDownMontage)
		{
			AnimInstance->Montage_Play(KneelingDownMontage, 1.0f);
			bIsKneeling = true;
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &APlayerCharacter::InteractingSavePoint);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, KneelingDownMontage);
			return;
		}

		if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			GI->SaveGameData();
		}
	}

	if (CurrentNPC)
	{
		CurrentNPC->StartTalk(this);
		return;
	}
}

void APlayerCharacter::InteractingSavePoint(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("세이브 포인트"));

	if (!OverlappingSavePoint) return;
	bIsInteracting = false;

	OverlappingSavePoint->SavePointInfo.bIsDiscovered = true;

	UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		GI->RegisterSavePoint(OverlappingSavePoint->SavePointInfo);
		GI->LastRestedSavePointID = OverlappingSavePoint->SavePointInfo.SavePointID;

		GI->SaveGameData();
	}

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	HPPotionCount = MaxHPPotionCount;
	FPPotionCount = MaxFPPotionCount;

	CombatStats.CurrentHP = DerivedStats.MaxHP;
	CombatStats.CurrentFP = DerivedStats.MaxFP;
	CombatStats.CurrentStamina = DerivedStats.MaxStamina;


	if (PlayerStatusWidgetInstance)
	{
		OnHPChanged.Broadcast(CombatStats.CurrentHP, DerivedStats.MaxHP);
		OnFPChanged.Broadcast(CombatStats.CurrentFP, DerivedStats.MaxFP);
		OnStaminaChanged.Broadcast(CombatStats.CurrentStamina, DerivedStats.MaxStamina);
	}
		

	if (!SavePointMenuInstance && SavePointMenuClass)
	{
		SavePointMenuInstance = CreateWidget<USavePointMenu>(GetWorld(), SavePointMenuClass);
		if (SavePointMenuInstance)
		{
			SavePointMenuInstance->OwningActor = this;
			SavePointMenuInstance->AddToViewport();

			// UI용 입력 모드로 전환
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(SavePointMenuInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;

				PC->SetIgnoreMoveInput(true);
				PC->SetIgnoreLookInput(true);
			}

			if (!bPausedBySavePoint)
			{
				SavedGlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
				bPausedBySavePoint = true;
			}
		}
	}
}


void APlayerCharacter::EndInteractSavePoint()
{
	if (bIsKneeling && OverlappingSavePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("세이브 포인트 상호작용 종료"));

		bIsKneeling = false;
		bIsInteracting = false;

		if (SavePointMenuInstance)
		{
			if (SavePointMenuInstance->IsInViewport())
			{
				SavePointMenuInstance->RemoveFromParent();
			}
			SavePointMenuInstance = nullptr;
		}

		if (bPausedBySavePoint)
		{
			if (!bPausedByInventory)
			{
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SavedGlobalTimeDilation);
			}
			bPausedBySavePoint = false;
		}

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.2f);
		}

		if (CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(false);
			CurrentWeapon->SetActorEnableCollision(true);
		}

		if (CurrentEquipedWidgetInstance)
			CurrentEquipedWidgetInstance->UpdatePotionCounts(HPPotionCount, FPPotionCount);

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);
		}
		SetStatusHUDVisible(true);


		return;

	}
}

void APlayerCharacter::OnEquipAnimationEnd(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("장비 획득"));

	if (!OverlappingItem || !OverlappingItem->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappingItem이 nullptr 또는 ItemData 없음"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>();

	if (Inventory)
	{
		int32 OutQuantity = 1;
		Inventory->AddItemByData(OverlappingItem->ItemData, OutQuantity);
	}

	OverlappingItem->Destroy();
	OverlappingItem = nullptr;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsInteracting = false;
}

void APlayerCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void APlayerCharacter::SetOverlappingSavePoint(ASavePoint* SavePoint)
{
	OverlappingSavePoint = SavePoint;
}

void APlayerCharacter::ReSetOverlappingItem()
{
	OverlappingItem = nullptr;
}

void APlayerCharacter::ReSetOverlappingSavePoint()
{
	OverlappingSavePoint = nullptr;
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
	if (bIsDead) return;

	if (bIsPopupInventory)
	{
		CloseInventory();
	}
	else
	{
		PopUpInventory();
	}
}

void APlayerCharacter::PopUpInventory()
{
	if (!bPausedByInventory)
	{
		SavedGlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
		bPausedByInventory = true;
	}

	if (!InventoryWidgetInstance && InventoryWidgetClass)
	{
		InventoryWidgetInstance = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
		InventoryWidgetInstance->OwningActor = this;
		InventoryWidgetInstance->AddToViewport();

	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}

	SetStatusHUDVisible(false);

	bIsPopupInventory = true;
}

void APlayerCharacter::CloseInventory()
{
	if (bPausedByInventory)
	{
		if (!bPausedBySavePoint)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SavedGlobalTimeDilation);
		}
		bPausedByInventory = false;
	}

	if (InventoryWidgetInstance)
	{
		InventoryWidgetInstance->RemoveFromParent();
		InventoryWidgetInstance = nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// 게임 입력으로 복원
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;

		// 캐릭터 입력 다시 허용
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
	}

	SetStatusHUDVisible(true);

	bIsPopupInventory = false;
}

UInventoryComponent* APlayerCharacter::GetInventoryComponent()
{
	return InventoryComponent;
}

void APlayerCharacter::ShowInteractionUI(const FText& InText)
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->SetHelpText(InText.ToString());
		InteractionWidgetInstance->SetIsEnabled(true);
		InteractionWidgetInstance->SetRenderOpacity(1.0f);
		InteractionWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}


void APlayerCharacter::ShowWeaponInfo_Implementation(UWeaponItemData* WeaponData)
{
	if (InventoryWidgetInstance && WeaponData)
	{
		InventoryWidgetInstance->UpdateWeaponInfo(WeaponData);
	}
}

void APlayerCharacter::HideInteractionUI()
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->SetIsEnabled(false);
		InteractionWidgetInstance->SetRenderOpacity(0.0f);
		InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerCharacter::TakeDamage(int32 DamageAmount)
{
	if (bIsDead) return;
	if (CombatStats.CurrentHP <= 0)
		return;

	const int32 Defense = CombatStats.Defense;
	const float DamageMultiplier = 100.f / (100.f + static_cast<float>(Defense));
	const int32 FinalDamage = FMath::Max(1, FMath::RoundToInt(DamageAmount * DamageMultiplier));

	CombatStats.CurrentHP = FMath::Clamp(CombatStats.CurrentHP - FinalDamage, 0, DerivedStats.MaxHP);


	OnHPChanged.Broadcast(CombatStats.CurrentHP, DerivedStats.MaxHP);

	if (CombatStats.CurrentHP <= 0)
	{
		// 사망 처리
		CombatStats.CurrentHP = 0;

		bIsDead = true;

		UE_LOG(LogTemp, Error, TEXT("플레이어 사망"));

		DisableInput(Cast<ATPRPlayerController>(GetController()));

		if (DeathScreenWidgetClass)
		{
			UUserWidget* DeathWidget = CreateWidget<UUserWidget>(GetWorld(), DeathScreenWidgetClass);
			if (DeathWidget)
			{
				DeathWidget->AddToViewport();
			}

			if (UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>())
			{
				if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
				{
					GI->CacheInventory(Inventory->GetAllItems());
					GI->CacheEquippedWeapon(Inventory->EquippedWeaponItem);
					GI->RegisterPlayerStatFromPlayer(this);
				}
			}
			
		}

		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APlayerCharacter::RespawnPlayer, 6.0f, false);

		return;
	}

	// 경직 애니메이션
	bIsRoll = false;
	bIsAttacking = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage && !bIsSkillActing)
	{
		AnimInstance->Montage_Play(HitReactMontage);
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
	if (!PlayerStatusWidgetInstance) return;

	if (!CurrentEquipedWidgetInstance) return;

	if (bVisible)
	{
		PlayerStatusWidgetInstance->SetVisibility(StatusHUDSavedVisibility);
		CurrentEquipedWidgetInstance->SetVisibility(StatusHUDSavedVisibility);
		CurrencyWidgetInstance->SetVisibility(StatusHUDSavedVisibility);
	}
	else
	{
		PlayerStatusWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		CurrentEquipedWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		CurrencyWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
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
	if (Amount <= 0) return;

	const int64 NewCurrency = static_cast<int64>(Currency) + static_cast<int64>(Amount);
	Currency = static_cast<int32>(FMath::Clamp<int64>(NewCurrency, 0, INT32_MAX));

	OnCurrencyChanged.Broadcast(Currency);
}

bool APlayerCharacter::SpendCurrency(int32 Amount)
{
	if (Amount <= 0) return true;
	if (Currency < Amount) return false;

	Currency -= Amount;
	OnCurrencyChanged.Broadcast(Currency);
	return true;
}


void APlayerCharacter::RefreshCurrentEquipped_Weapon(UTexture2D* WeaponIconTexture)
{
	CurrentWeaponIcon = WeaponIconTexture;
	if (CurrentEquipedWidgetInstance)
	{
		CurrentEquipedWidgetInstance->UpdateWeaponIcon(WeaponIconTexture);
	}
}

void APlayerCharacter::RefreshCurrentEquipped_Potion(UTexture2D* PotionIconTexture, int32 NewCount)
{
	HPPotionCount = NewCount;
	if (CurrentEquipedWidgetInstance)
	{
		CurrentEquipedWidgetInstance->UpdatePotion(NewCount);
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

void APlayerCharacter::RecalculateStatsAfterLevelUp(bool bRefillHPFPStamina /*= false*/)
{
	// 기존 최대치 저장
	const int32 OldMaxHP = DerivedStats.MaxHP;
	const int32 OldMaxFP = DerivedStats.MaxFP;
	const int32 OldMaxStamina = DerivedStats.MaxStamina;

	// 파생치 재계산
	CalculateDerivedStats();

	// 현재 수치 동기화 (회복 or 비율 유지)
	auto ScaleKeepRatio = [](int32 Curr, int32 OldMax, int32 NewMax)
		{
			if (OldMax <= 0) return FMath::Clamp(Curr, 0, NewMax);
			const float Ratio = static_cast<float>(Curr) / static_cast<float>(OldMax);
			return FMath::Clamp(FMath::RoundToInt(Ratio * static_cast<float>(NewMax)), 0, NewMax);
		};

	if (bRefillHPFPStamina)
	{
		CombatStats.CurrentHP = DerivedStats.MaxHP;
		CombatStats.CurrentFP = DerivedStats.MaxFP;
		CombatStats.CurrentStamina = DerivedStats.MaxStamina;
	}
	else
	{
		CombatStats.CurrentHP = ScaleKeepRatio(CombatStats.CurrentHP, OldMaxHP, DerivedStats.MaxHP);
		CombatStats.CurrentFP = ScaleKeepRatio(CombatStats.CurrentFP, OldMaxFP, DerivedStats.MaxFP);
		CombatStats.CurrentStamina = ScaleKeepRatio(CombatStats.CurrentStamina, OldMaxStamina, DerivedStats.MaxStamina);
	}

	if (CurrentWeapon)
	{
		ApplyWeaponStats(CurrentWeapon);
	}
	else
	{
		ResetCombatStats();
	}
	CombatStats.Poise = DerivedStats.Poise;

	if (PlayerStatusWidgetInstance)
	{
		PlayerStatusWidgetInstance->UpdateBarLengths(
			DerivedStats.MaxHP,
			DerivedStats.MaxFP,
			DerivedStats.MaxStamina
		);
		OnHPChanged.Broadcast(CombatStats.CurrentHP, DerivedStats.MaxHP);
		OnFPChanged.Broadcast(CombatStats.CurrentFP, DerivedStats.MaxFP);
		OnStaminaChanged.Broadcast(CombatStats.CurrentStamina, DerivedStats.MaxStamina);
	}
}

void APlayerCharacter::FillSaveData(FPlayerStatSaveData& OutSaveData) const
{
	OutSaveData.BaseAttributes = CharacterAttributes;
	OutSaveData.DerivedStats = DerivedStats;
	//OutSaveData.CombatStats = CombatStats;
	OutSaveData.Currency = Currency;
	OutSaveData.SpentCurrencyOnStats = SpentCurrencyOnStats;
}

void APlayerCharacter::ApplySaveData(const FPlayerStatSaveData& InSaveData)
{
	CharacterAttributes = InSaveData.BaseAttributes;
	Currency = InSaveData.Currency;
	SpentCurrencyOnStats = InSaveData.SpentCurrencyOnStats;

	CalculateDerivedStats();

	CombatStats.CurrentHP = DerivedStats.MaxHP;
	CombatStats.CurrentFP = DerivedStats.MaxFP;
	CombatStats.CurrentStamina = DerivedStats.MaxStamina;

	ResetCombatStats();
	CombatStats.Poise = DerivedStats.Poise;

	if (CurrentWeapon)
	{
		ApplyWeaponStats(CurrentWeapon);
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
	CurrentNPC = InNPC;
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