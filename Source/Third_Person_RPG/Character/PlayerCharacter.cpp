// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Third_Person_RPG/Data/MMComboActionData.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
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

	// 컨트롤러의 Rotation에 영향 X
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 움직임에 따른 회전 On
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.f);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MMCapsule"));

	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));



	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
	

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void APlayerCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void APlayerCharacter::RollStart()
{
	// 구르기 중이면 리턴
	if (bIsRoll) return;

	// 애님 인스턴스 가져오기
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// Roll Check (구르기 활성화)
		bIsRoll = true;

		// 몽타주 재생
		AnimInstance->Montage_Play(RollMontage, 1.3f);

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
	if (bIsRoll) return;

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

void APlayerCharacter::ComboStart()
{
	CurrentComboCount = 1;

	// 공격 시 플레이어 이동 불가
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// TODO : 공격 속도가 추가되면 값 가져와 지정하기
	const float AttackSpeedRate = 1.0f;


	// 애님 인스턴스 가져오기
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 몽타주 재생
		AnimInstance->Montage_Play(BasicComboMontage, AttackSpeedRate);

		// 몽타주 재생 종료 바인딩
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::ComboEnd);

		// BasicComboMontage가 종료되면 EndDelegate에 연동된 ComboEnd함수 호출
		AnimInstance->Montage_SetEndDelegate(EndDelegate, BasicComboMontage);

		// 타이머 초기화
		ComboTimerHandle.Invalidate();
		// 타이머 설정
		SetComboTimer();
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

	if (bHasComboInput)
	{
		//콤보 수 증가
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, BasicComboData->MaxComboCount);

		// 애님 인스턴스 가져오기
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 다음 섹션의 이름
			FName SectionName = *FString::Printf(TEXT("%s%d"), *BasicComboData->SectionPrefix, CurrentComboCount);

			// 다음 섹션으로 이동하기
			AnimInstance->Montage_JumpToSection(SectionName, BasicComboMontage);

			// 타이머 재설정
			SetComboTimer();
			// 콤보 입력 판별 초기화
			// 콤보 입력 판별 초기화
			bHasComboInput = false;
		}
	}
}

void APlayerCharacter::SetComboTimer()
{
	// 인덱스 조정
	// * 콤보 인덱스 : 1, 2, 3 
	// * 배열 인덱스 : 0, 1, 2
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

