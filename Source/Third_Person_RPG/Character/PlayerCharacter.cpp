// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Third_Person_RPG/Data/MMComboActionData.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Data/SkillData.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"	
#include "Third_Person_RPG/UI/InventoryUI/InventoryWidget.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/UI/SavePointUI/SavePointMenu.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Blueprint/UserWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/UI/PlayerStatusWidget.h" 

#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

// Sets default values
APlayerCharacter::APlayerCharacter()
{
#pragma region Input

	//Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>IMC_BasicRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input_Action/IMC_BasicPlayer.IMC_BasicPlayer'"));
	if (IMC_BasicRef.Object)
	{
		IMC_Basic = IMC_BasicRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_BasicMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_BasicMove.IA_BasicMove'"));
	if (IA_BasicMoveRef.Object)
	{
		IA_BasicMove = IA_BasicMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_BasicLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_BasicLook.IA_BasicLook'"));
	if (IA_BasicLookRef.Object)
	{
		IA_BasicLook = IA_BasicLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_SprintRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_Sprint.IA_Sprint'"));
	if (IA_SprintRef.Object)
	{
		IA_Sprint = IA_SprintRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_RollRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_Roll.IA_Roll'"));
	if (IA_RollRef.Object)
	{
		IA_Roll = IA_RollRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_AttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_Attack.IA_Attack'"));
	if (IA_AttackRef.Object)
	{
		IA_Attack = IA_AttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_SkillRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_Skill.IA_Skill'"));
	if (IA_SkillRef.Object)
	{
		IA_Skill = IA_SkillRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_UnEquipWeapon_TestRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_UnEquipWeapon_Test.IA_UnEquipWeapon_Test'"));
	if (IA_UnEquipWeapon_TestRef.Object)
	{
		IA_UnEquipWeapon_Test = IA_UnEquipWeapon_TestRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_InteractionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_Interaction.IA_Interaction'"));
	if (IA_InteractionRef.Object)
	{
		IA_Interaction = IA_InteractionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_InventoryRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input_Action/IA_Inventory.IA_Inventory'"));
	if (IA_InventoryRef.Object)
	{
		IA_Inventory = IA_InventoryRef.Object;
	}

#pragma endregion

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, 90.0f, 0.0f)));

	SpringArmComp->SetupAttachment(GetMesh());

	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	SpringArmComp->bUsePawnControlRotation = true;

	GetCharacterMovement()->bAllowAnyoneToDestroyMe = true;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

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

	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CalculateDerivedStats();
	InitializeCombatStats();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController && IMC_Basic)
	{
		//서브 시스템 불러오기
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//매핑 컨텍스트 추가
			SubSystem->AddMappingContext(IMC_Basic, 0);

			//입력 시작
			EnableInput(PlayerController);
		}

	}

	if (InteractionWidgetClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			InteractionWidgetInstance = CreateWidget<UInteractionWidget>(PC, InteractionWidgetClass);
			if (InteractionWidgetInstance)
			{
				InteractionWidgetInstance->AddToViewport();
				InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (const UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
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
			for (const FInventoryItemSaveData& SaveData : GI->GetCachedInventory())
			{
				bool bShouldEquip = SaveData.bEquipped;

				UInventoryItem* AddedItem = InventoryComponent->AddItemByData(SaveData.ItemData, SaveData.Quantity, bShouldEquip);
			}
		}
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

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(IA_BasicMove, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicMove);
	EnhancedInputComponent->BindAction(IA_BasicLook, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicLook);
	EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &APlayerCharacter::BeginSprint);
	EnhancedInputComponent->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &APlayerCharacter::EndSprint);
	EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicAttack);
	EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &APlayerCharacter::RollStart);
	EnhancedInputComponent->BindAction(IA_Skill, ETriggerEvent::Started, this, &APlayerCharacter::SkillStart);
	EnhancedInputComponent->BindAction(IA_Interaction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
	EnhancedInputComponent->BindAction(IA_Inventory, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SprintStaminaTick(DeltaTime);

	StaminaRegenTick(DeltaTime);
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

void APlayerCharacter::ApplyWeaponStats(ATPRWeapon* Weapon)
{
	if (!Weapon || !Weapon->ItemData) return;

	const UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Weapon->ItemData);
	if (!WeaponItemData) return;

	const FWeaponStatData& Stat = WeaponItemData->WeaponStats;


	ResetCombatStats(); // 무기 해제 시와 동일하게 초기화


	// Physical = 근력 + 기량 보정
	float PhysicalScaling =
		ConvertScalingToMultiplier(Stat.StrengthScaling) * CharacterAttributes.Strength +
		ConvertScalingToMultiplier(Stat.DexterityScaling) * CharacterAttributes.Dexterity;

	// Magic = 지력 보정
	float MagicScaling =
		ConvertScalingToMultiplier(Stat.IntelligenceScaling) * CharacterAttributes.Intelligence;

	// Fire = 신비 보정
	float FireScaling =
		ConvertScalingToMultiplier(Stat.ArcaneScaling) * CharacterAttributes.Arcane;

	// Lightning / Holy = 신앙 보정
	float LightningScaling =
		ConvertScalingToMultiplier(Stat.FaithScaling) * CharacterAttributes.Faith;

	float HolyScaling = LightningScaling; // 동일하게 신앙 보정 사용

	int32 TotalPhysical = Stat.Physical + FMath::RoundToInt(PhysicalScaling);
	int32 TotalMagic = Stat.Magic + FMath::RoundToInt(MagicScaling);
	int32 TotalFire = Stat.Fire + FMath::RoundToInt(FireScaling);
	int32 TotalLightning = Stat.Lightning + FMath::RoundToInt(LightningScaling);
	int32 TotalHoly = Stat.Holy + FMath::RoundToInt(HolyScaling);

	// 총합 공격력 = 속성 포함 전체
	int32 TotalAttackPower = TotalPhysical + TotalMagic + TotalFire + TotalLightning + TotalHoly;

	CombatStats.AttackPower = TotalAttackPower;


	UE_LOG(LogTemp, Warning, TEXT("총 공격력: %d (물리 %d / 마법 %d / 화염 %d / 번개 %d / 신성 %d)"),
		TotalAttackPower, TotalPhysical, TotalMagic, TotalFire, TotalLightning, TotalHoly);
}

void APlayerCharacter::ResetCombatStats()
{
	CombatStats.AttackPower = 50 + (CharacterAttributes.Strength * 2);  // 베이스 값
	CombatStats.Defense = 10 + FMath::RoundToInt(CharacterAttributes.Strength * 1.5f);     // 기본값
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

void APlayerCharacter::BasicAttack()
{
	if (bIsInteracting || bIsRoll || bIsSkillActing || bIsInteracting || bIsKneeling || bIsPopupInventory) return;

	if (CurrentComboCount == 0)
	{
		ComboStart();
		bIsAttacking = true;
		return;
	}

	if (ComboTimerHandle.IsValid())
	{
		bHasComboInput = true;
	}
	else
	{
		bHasComboInput = false;
	}
}

void APlayerCharacter::SkillStart()
{
	if (bIsInteracting || bIsRoll || !SkillData || !SkillData->SkillMontage || bIsSkillActing) return;

	if (bSkillConsumesFP)
	{
		const int32 FPCost = GetCurrentSkillFPCost();
		if (FPCost > 0)
		{
			if (!TryConsumeFP(FPCost))
			{
				return;
			}
		}
	}

	if (CombatStats.CurrentStamina < StaminaCost_Skill) return;
	ConsumeStamina(StaminaCost_Skill);

	// 공격 시 플레이어 이동 불가
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 애님 인스턴스 가져오기
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{
		bIsSkillActing = true;
		bIsAttacking = true;

		// 몽타주 재생
		AnimInstance->Montage_Play(SkillData->SkillMontage, SkillData->SkillPlayRate);

		// 몽타주 재생 종료 바인딩
		FOnMontageEnded EndDelegate;

		// RollMontage 종료 시 EndDelegate에 연동된 함수 호출
		AnimInstance->Montage_SetEndDelegate(EndDelegate, SkillData->SkillMontage);

	}
}

void APlayerCharacter::SpawnSkillEffect()
{
	if (!SkillData || !SkillData->SkillEffect) return;

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	switch (SkillData->SpawnType)
	{
	case ESkillEffectSpawnType::Self:
		SpawnLocation = GetActorLocation();
		break;

	case ESkillEffectSpawnType::Forward:
		SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f;
		break;

	case ESkillEffectSpawnType::Ground:
		SpawnLocation = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		break;

	case ESkillEffectSpawnType::Custom:
		// 혹시 필요하다면 SkillData에 별도 커스텀 위치 변수 추가 가능
		break;

	default:
		break;
	}

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		SkillData->SkillEffect,
		SpawnLocation,
		SpawnRotation
	);
}

void APlayerCharacter::ComboStart()
{
	if (!TryConsumeStamina(StaminaCost_Attack)) return;

	if (CurrentWeapon)
	{
		CurrentComboCount = 1;

		// 공격 시 플레이어 이동 불가
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		// 애님 인스턴스 가져오기
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(WeaponComboData->ComboMontage, WeaponComboData->ComnboPlayRate);

			// 몽타주 재생 종료 바인딩
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &APlayerCharacter::ComboEnd);

			// BasicComboMontage가 종료되면 EndDelegate에 연동된 ComboEnd함수 호출
			AnimInstance->Montage_SetEndDelegate(EndDelegate, WeaponComboData->ComboMontage);

			// 타이머 초기화
			ComboTimerHandle.Invalidate();
			// 타이머 설정
			SetComboTimer();
		}
	}
	else
	{
		CurrentComboCount = 1;

		// 공격 시 플레이어 이동 불가
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);


		// 애님 인스턴스 가져오기
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 몽타주 재생
			AnimInstance->Montage_Play(BasicComboData->ComboMontage, BasicComboData->ComnboPlayRate);

			// 몽타주 재생 종료 바인딩
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &APlayerCharacter::ComboEnd);

			// BasicComboMontage가 종료되면 EndDelegate에 연동된 ComboEnd함수 호출
			AnimInstance->Montage_SetEndDelegate(EndDelegate, BasicComboData->ComboMontage);

			// 타이머 초기화
			ComboTimerHandle.Invalidate();
			// 타이머 설정
			SetComboTimer();
		}
	}
	
}

void APlayerCharacter::ComboEnd(UAnimMontage* Montage, bool IsEnded)
{
	CurrentComboCount = 0;

	bHasComboInput = false;

	bIsAttacking = false;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void APlayerCharacter::ComboCheck()
{
	ComboTimerHandle.Invalidate();

	auto ConsumePerHitOrStop = [this]() -> bool
		{
			if (!bRequireStaminaForComboContinue)
			{
				if (HasStamina(StaminaCost_AttackPerHit))
				{
					ConsumeStamina(StaminaCost_AttackPerHit);
				}
				return true;
			}

			if (!TryConsumeStamina(StaminaCost_AttackPerHit))
			{
				bHasComboInput = false;
				bIsAttacking = false;
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

				return false;
			}
			return true;
		};

	if (CurrentWeapon)
	{
		if (bHasComboInput)
		{
			if (!ConsumePerHitOrStop()) return;

			//콤보 수 증가
			CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, WeaponComboData->MaxComboCount);

			// 애님 인스턴스 가져오기
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				// 다음 섹션의 이름
				FName SectionName = *FString::Printf(TEXT("%s%d"), *WeaponComboData->SectionPrefix, CurrentComboCount);

				// 다음 섹션으로 이동하기
				AnimInstance->Montage_JumpToSection(SectionName, WeaponComboData->ComboMontage);

				// 타이머 재설정
				SetComboTimer();
				// 콤보 입력 판별 초기화
				bHasComboInput = false;
			}
		}
	}
	else
	{
		if (bHasComboInput)
		{
			if (!ConsumePerHitOrStop()) return;

			//콤보 수 증가
			CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, BasicComboData->MaxComboCount);

			// 애님 인스턴스 가져오기
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				// 다음 섹션의 이름
				FName SectionName = *FString::Printf(TEXT("%s%d"), *BasicComboData->SectionPrefix, CurrentComboCount);

				// 다음 섹션으로 이동하기
				AnimInstance->Montage_JumpToSection(SectionName, BasicComboData->ComboMontage);

				// 타이머 재설정
				SetComboTimer();
				// 콤보 입력 판별 초기화
				bHasComboInput = false;
			}
		}
	}
	
}

void APlayerCharacter::SetComboTimer()
{
	// 인덱스 조정
	// * 콤보 인덱스 : 1, 2, 3 
	// * 배열 인덱스 : 0, 1, 2

	if (CurrentWeapon)
	{
		int32 ComboIndex = CurrentComboCount - 1;

		// 인덱스가 유효한지 체크
		if (WeaponComboData->ComboFrame.IsValidIndex(ComboIndex))
		{
			// TODO : 공격 속도가 추가되면 값 가져와 지정하기
			const float AttackSpeedRate = 1.0f;

			// 실제 콤보가 입력될 수 있는 시간 구하기
			float ComboAvailableTime = (WeaponComboData->ComboFrame[ComboIndex] / WeaponComboData->FrameRate) / AttackSpeedRate;

			// 타이머 설정하기
			if (ComboAvailableTime > 0.0f)
			{
				// ComboAvailableTime시간이 지나면 ComboCheck() 함수 호출
				GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &APlayerCharacter::ComboCheck, ComboAvailableTime, false);
			}
		}
	}
	else
	{
		int32 ComboIndex = CurrentComboCount - 1;

		// 인덱스가 유효한지 체크
		if (BasicComboData->ComboFrame.IsValidIndex(ComboIndex))
		{
			// TODO : 공격 속도가 추가되면 값 가져와 지정하기
			const float AttackSpeedRate = 1.0f;

			// 실제 콤보가 입력될 수 있는 시간 구하기
			float ComboAvailableTime = (BasicComboData->ComboFrame[ComboIndex] / BasicComboData->FrameRate) / AttackSpeedRate;

			// 타이머 설정하기
			if (ComboAvailableTime > 0.0f)
			{
				// ComboAvailableTime시간이 지나면 ComboCheck() 함수 호출
				GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &APlayerCharacter::ComboCheck, ComboAvailableTime, false);
			}
		}
	}
	
}

void APlayerCharacter::BaseAttackCheck()
{
	if (CurrentWeapon == nullptr)
	{
		TArray<FOverlapResult> OverlapResults;

		// 충돌 탐지를 위한 시작 지점
		FVector Start = GetActorLocation() + (GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius());

		// 충돌 탐지 끝 지점
		FVector End = Start + (GetActorForwardVector() * BasicComboData->AttackRange);

		// 오버랩 파라미터 설정
		FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackOverlap), false, this);

		// 충돌 셰이프 설정 (캡슐)
		FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(BasicComboData->AttackRadius, BasicComboData->AttackRange * 0.5f);

		// 캡슐 중심 위치 계산
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat();

		// 오버랩 수행
		bool bHasHit = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			CapsuleOrigin,
			CapsuleRotation,
			CHANNEL_ACTION,  // ECollisionChannel::ECC_GameTraceChannel2
			CollisionShape,
			Params
		);

		// 판정 결과 처리
		if (bHasHit)
		{
			TSet<AEnemyCharacter*> HitEnemies;

			for (const FOverlapResult& Result : OverlapResults)
			{
				if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Result.GetActor()))
				{
					if (!HitEnemies.Contains(Enemy))
					{
						HitEnemies.Add(Enemy);
						UE_LOG(LogTemp, Warning, TEXT("Monster Damaged via Overlap"));
						Enemy->TakeDamage(CombatStats.AttackPower);
					}
				}
			}
		}

		// 디버그 캡슐 시각화
		FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, BasicComboData->AttackRange * 0.5f, BasicComboData->AttackRadius, CapsuleRotation, DrawColor, false, 3.0f);
	}
	
}

void APlayerCharacter::EnableWeaponHitBox()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EnableHitBox();
	}
}

void APlayerCharacter::DisableWeaponHitBox()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->DisableHitBox();
	}
}


void APlayerCharacter::SkillAttackCheck()
{
	if (!SkillData) return;

	SpawnSkillEffect();

	TArray<FOverlapResult> OverlapResults;

	// 기본 값
	FVector Start = GetActorLocation();
	FVector End = Start;

	// 방향 계산
	FVector Forward = GetActorForwardVector();

	switch (SkillData->SpawnType)
	{
	case ESkillEffectSpawnType::Forward:
		Start = GetActorLocation() + Forward * 200.0f;
		End = Start + Forward * SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Self:
		Start = GetActorLocation();
		End = Start + Forward * SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Ground:
		Start = GetActorLocation() - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		End = Start + Forward * SkillData->SkillRange;
		break;

	case ESkillEffectSpawnType::Custom:
		// 필요시 커스텀 위치 로직 추가
		break;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	bool bHasHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		Start,
		FQuat::Identity,
		CHANNEL_ACTION,
		FCollisionShape::MakeSphere(SkillData->SkillRadius),
		Params
	);

	if (bHasHit)
	{
		TSet<AEnemyCharacter*> HitEnemies;

		for (const FOverlapResult& Result : OverlapResults)
		{
			if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Result.GetActor()))
			{
				if (!HitEnemies.Contains(Enemy))
				{
					HitEnemies.Add(Enemy);
					UE_LOG(LogTemp, Warning, TEXT("Monster Damaged via Overlap"));
					Enemy->TakeDamage(CombatStats.AttackPower);
				}
			}
		}
	}

	// Capsule 디버그 시각화
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = SkillData->SkillRange * 0.5f;
	FColor DrawColor = bHasHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(
		GetWorld(),
		CapsuleOrigin,
		CapsuleHalfHeight,
		SkillData->SkillRadius,
		FRotationMatrix::MakeFromZ(Forward).ToQuat(),
		DrawColor,
		false,
		3.0f
	);


	bIsSkillActing = false;
	bIsAttacking = false;

	// 이동 다시 가능하게
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}


void APlayerCharacter::Interact()
{
	if (bIsRoll || bIsSkillActing || bIsAttacking || bIsInteracting) return;
	UE_LOG(LogTemp, Warning, TEXT("상호작용 키 입력됨"));

	UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>();


	if (OverlappingItem && OverlappingItem->ItemData)
	{
		FPrimaryAssetId AssetId = OverlappingItem->ItemData->GetPrimaryAssetId();

		// 2. GameInstance에 등록
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
	}

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

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

bool APlayerCharacter::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
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

void APlayerCharacter::EquipWeapon_Implementation(UInventoryItem* WeaponItem)
{
	if (!WeaponItem || !WeaponItem->ItemData) return;

	const auto ItemData = Cast<UWeaponItemData>(WeaponItem->ItemData);
	if (!ItemData || !ItemData->WeaponClass) return;

	// 이전에 들고 있던 무기 제거
	if (CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
		SkillData = nullptr;
		WeaponComboData = nullptr;

		InventoryComponent->EquippedWeaponItem = nullptr;
	}
	// 새 무기 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATPRWeapon* NewWeapon = GetWorld()->SpawnActor<ATPRWeapon>(
		ItemData->WeaponClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator
	);

	if (NewWeapon)
	{
		SetWeapon(NewWeapon);

		InventoryComponent->EquippedWeaponItem = WeaponItem;
	}

	WeaponItem->bEquipped = true;
}


void APlayerCharacter::UnEquipWeapon_Implementation(UInventoryItem* WeaponItem)
{

	if (CurrentWeapon)
	{
		// 1. 먼저 무기를 소켓에서 분리
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. 월드에서 제거
		CurrentWeapon->Destroy();

		// 3. 참조 해제
		CurrentWeapon = nullptr;
		SkillData = nullptr;
		WeaponComboData = nullptr;

		InventoryComponent->EquippedWeaponItem = nullptr;

		ResetCombatStats();

		UE_LOG(LogTemp, Warning, TEXT("무기 해제됨"));
	}
}
void APlayerCharacter::ToggleInventory()
{
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
	if (!InventoryWidgetInstance && InventoryWidgetClass)
	{
		InventoryWidgetInstance = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
		InventoryWidgetInstance->OwningActor = this;
		InventoryWidgetInstance->AddToViewport();

	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// 입력 모드를 UI + Game으로 설정 (I 키는 계속 인식하게)
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

		// 캐릭터 입력 차단
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}

	SetStatusHUDVisible(false);

	bIsPopupInventory = true;
}

void APlayerCharacter::CloseInventory()
{
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
		InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerCharacter::TakeDamage(int32 DamageAmount)
{
	if (CombatStats.CurrentHP <= 0)
		return;

	const int32 Defense = CombatStats.Defense;
	const float DamageMultiplier = 100.f / (100.f + static_cast<float>(Defense));
	const int32 FinalDamage = FMath::Max(1, FMath::RoundToInt(DamageAmount * DamageMultiplier));

	CombatStats.CurrentHP = FMath::Clamp(CombatStats.CurrentHP - FinalDamage, 0, DerivedStats.MaxHP);

	if (PlayerStatusWidgetInstance)
	{
		PlayerStatusWidgetInstance->UpdateHP(CombatStats.CurrentHP, DerivedStats.MaxHP);
	}

	if (CombatStats.CurrentHP <= 0)
	{
		// 사망 처리
		CombatStats.CurrentHP = 0;
		UE_LOG(LogTemp, Error, TEXT("플레이어 사망"));

		DisableInput(Cast<APlayerController>(GetController()));
		return;
	}

	// 경직 애니메이션
	bIsRoll = false;
	bIsSkillActing = false;
	bIsAttacking = false;
	bIsInteracting = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

void APlayerCharacter::ConsumeStamina(int32 Amount)
{
	CombatStats.CurrentStamina = FMath::Clamp(CombatStats.CurrentStamina - Amount, 0, DerivedStats.MaxStamina);
	if (PlayerStatusWidgetInstance)
	{
		PlayerStatusWidgetInstance->UpdateStamina(CombatStats.CurrentStamina, DerivedStats.MaxStamina);
	}
}

void APlayerCharacter::RestoreStaminaTick(int32 AmountPerTick)
{
	CombatStats.CurrentStamina = FMath::Clamp(CombatStats.CurrentStamina + AmountPerTick, 0, DerivedStats.MaxStamina);
	if (PlayerStatusWidgetInstance)
	{
		PlayerStatusWidgetInstance->UpdateStamina(CombatStats.CurrentStamina, DerivedStats.MaxStamina);
	}
}

int32 APlayerCharacter::GetStaminaRegenPerSecond() const
{
	const float Half = DerivedStats.MaxEquipLoad * 0.5f;
	const float W = GetCurrentWeaponWeight();

	if (W <= Half - 5.f)
	{
		return StaminaRegen_Light;
	}
	if (W > Half - 5.f && W < Half + 5.f)
	{
		return StaminaRegen_Medium;
	}
	return StaminaRegen_Heavy;
}

float APlayerCharacter::GetCurrentWeaponWeight() const
{
	if (CurrentWeapon && CurrentWeapon->ItemData)
	{
		if (const UWeaponItemData* WData = Cast<UWeaponItemData>(CurrentWeapon->ItemData))
		{
			return WData->WeaponStats.Weight;
		}
	}
	return 0.f;
}

void APlayerCharacter::StaminaRegenTick(float DeltaSeconds)
{
	// 비활성화면 종료
	if (!bEnableFrameStaminaRegen) return;

	if (bBlockRegenWhileSprinting && bIsSprinting) return;

	// 이미 최대면 종료
	if (CombatStats.CurrentStamina >= DerivedStats.MaxStamina) return;

	// 초당 회복량(정수)을 프레임 환산
	const int32 RegenPerSec = GetStaminaRegenPerSecond(); // (무게 조건에 따라 70/50/30 반환)
	if (RegenPerSec <= 0) return;

	// 누적: 초당 * Delta + 잔여분
	StaminaRegenAccum += static_cast<float>(RegenPerSec) * DeltaSeconds;

	// 정수 부분만 반영, 소수는 다음 프레임으로 이월
	const int32 RegenWhole = FMath::FloorToInt(StaminaRegenAccum);
	if (RegenWhole > 0)
	{
		StaminaRegenAccum -= static_cast<float>(RegenWhole);

		// 실제 정수만큼 회복 (UI 갱신 포함)
		RestoreStaminaTick(RegenWhole);
	}
}

bool APlayerCharacter::HasStamina(int32 Amount) const
{
	return CombatStats.CurrentStamina >= Amount;
}

bool APlayerCharacter::TryConsumeStamina(int32 Amount)
{
	if (!HasStamina(Amount)) return false;
	ConsumeStamina(Amount); 
	return true;
}

void APlayerCharacter::SetStatusHUDVisible(bool bVisible)
{
	if (!PlayerStatusWidgetInstance) return;

	if (bVisible)
	{
		PlayerStatusWidgetInstance->SetVisibility(StatusHUDSavedVisibility);
	}
	else
	{
		PlayerStatusWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void APlayerCharacter::SprintStaminaTick(float DeltaSeconds)
{
	if (!bIsSprinting) return;

	// 스태미너가 바닥나면 자동으로 스프린트 종료
	if (CombatStats.CurrentStamina <= 0)
	{
		EndSprint();
		return;
	}

	// 초당 소모량을 프레임 단위로 누적
	const int32 DrainPerSec = StaminaCost_SprintPerSecond;
	if (DrainPerSec <= 0) return;

	SprintDrainAccum += static_cast<float>(DrainPerSec) * DeltaSeconds;

	// 정수 부분만 소모, 소수는 누적 유지
	const int32 DrainWhole = FMath::FloorToInt(SprintDrainAccum);
	if (DrainWhole > 0)
	{
		SprintDrainAccum -= static_cast<float>(DrainWhole);
		ConsumeStamina(DrainWhole); // UI 자동 갱신됨

		if (CombatStats.CurrentStamina <= 0)
		{
			EndSprint();
		}
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