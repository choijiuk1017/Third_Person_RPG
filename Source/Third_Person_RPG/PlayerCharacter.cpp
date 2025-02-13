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


// Sets default values
APlayerCharacter::APlayerCharacter()
{
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



	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.f);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MMCapsule"));

	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));


	static ConstructorHelpers::FObjectFinder<UInputMappingContext>IMC_BasicRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/IMC_BasicPlayer.IMC_BasicPlayer'"));
	if (IMC_BasicRef.Object)
	{
		IMC_Basic = IMC_BasicRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_AttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/IA_Attack.IA_Attack'"));
	if (IA_AttackRef.Object)
	{
		IA_Attack = IA_AttackRef.Object;
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController && IMC_Basic)
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(IMC_Basic, 0);

			EnableInput(PlayerController);
		}

	}


}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::MoveForward(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// ���� ã��
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// ���� ���� ���ϱ�
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// ������ ã��
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// ������ ���� ���ϱ�
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// �ش� �������� �̵� �߰�
		AddMovementInput(Direction, AxisValue);
	}
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

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::EndSprint);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::RollStart);

	EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicAttack);
	//PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::BasicAttack);
}

