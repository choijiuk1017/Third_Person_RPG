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

	// ��Ʈ�ѷ��� Rotation�� ���� X
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// �����ӿ� ���� ȸ�� On
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
		//���� �ý��� �ҷ�����
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//���� ���ؽ�Ʈ �߰�
			SubSystem->AddMappingContext(IMC_Basic, 0);

			//�Է� ����
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
	// �Է¹��� Value�κ��� MovementVector ��������
	FVector2D MovementVector = Value.Get<FVector2D>();

	// ��Ʈ�ѷ��� ȸ�� �� Yaw(Z)�� ������ ����
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// ȸ��(Yaw)�� ������� ���� �� ������ ������ �޾ƿ��� (X : ����, Y : ������)
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Movement�� �� ���� (����, �̵���)
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void APlayerCharacter::BasicLook(const FInputActionValue& Value)
{
	// �Է¹��� Value�κ��� LookVector ��������
	FVector2D LookVector = Value.Get<FVector2D>();

	// Controller�� �� ����
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
	// ������ ���̸� ����
	if (bIsRoll) return;

	// �ִ� �ν��Ͻ� ��������
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// Roll Check (������ Ȱ��ȭ)
		bIsRoll = true;

		// ��Ÿ�� ���
		AnimInstance->Montage_Play(RollMontage, 1.3f);

		// ��Ÿ�� ��� ���� ���ε�
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::RollEnd);

		// RollMontage ���� �� EndDelegate�� ������ �Լ� ȣ��
		AnimInstance->Montage_SetEndDelegate(EndDelegate, RollMontage);
	}
}

void APlayerCharacter::RollEnd(class UAnimMontage* Montage, bool IsEnded)
{
	// Roll UnCheck (������ ��Ȱ��ȭ)
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

	// ���� �� �÷��̾� �̵� �Ұ�
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// TODO : ���� �ӵ��� �߰��Ǹ� �� ������ �����ϱ�
	const float AttackSpeedRate = 1.0f;


	// �ִ� �ν��Ͻ� ��������
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// ��Ÿ�� ���
		AnimInstance->Montage_Play(BasicComboMontage, AttackSpeedRate);

		// ��Ÿ�� ��� ���� ���ε�
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &APlayerCharacter::ComboEnd);

		// BasicComboMontage�� ����Ǹ� EndDelegate�� ������ ComboEnd�Լ� ȣ��
		AnimInstance->Montage_SetEndDelegate(EndDelegate, BasicComboMontage);

		// Ÿ�̸� �ʱ�ȭ
		ComboTimerHandle.Invalidate();
		// Ÿ�̸� ����
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
		//�޺� �� ����
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, BasicComboData->MaxComboCount);

		// �ִ� �ν��Ͻ� ��������
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// ���� ������ �̸�
			FName SectionName = *FString::Printf(TEXT("%s%d"), *BasicComboData->SectionPrefix, CurrentComboCount);

			// ���� �������� �̵��ϱ�
			AnimInstance->Montage_JumpToSection(SectionName, BasicComboMontage);

			// Ÿ�̸� �缳��
			SetComboTimer();
			// �޺� �Է� �Ǻ� �ʱ�ȭ
			// �޺� �Է� �Ǻ� �ʱ�ȭ
			bHasComboInput = false;
		}
	}
}

void APlayerCharacter::SetComboTimer()
{
	// �ε��� ����
	// * �޺� �ε��� : 1, 2, 3 
	// * �迭 �ε��� : 0, 1, 2
	int32 ComboIndex = CurrentComboCount - 1;

	// �ε����� ��ȿ���� üũ
	if (BasicComboData->ComboFrame.IsValidIndex(ComboIndex))
	{
		// TODO : ���� �ӵ��� �߰��Ǹ� �� ������ �����ϱ�
		const float AttackSpeedRate = 1.0f;

		// ���� �޺��� �Էµ� �� �ִ� �ð� ���ϱ�
		float ComboAvailableTime = (BasicComboData->ComboFrame[ComboIndex] / BasicComboData->FrameRate) / AttackSpeedRate;

		// Ÿ�̸� �����ϱ�
		if (ComboAvailableTime > 0.0f)
		{
			// ComboAvailableTime�ð��� ������ ComboCheck() �Լ� ȣ��
			GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &APlayerCharacter::ComboCheck, ComboAvailableTime, false);
		}
	}
}

