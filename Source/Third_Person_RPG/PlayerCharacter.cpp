// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


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

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
		// 앞쪽 찾기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 앞쪽 벡터 구하기
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void APlayerCharacter::MoveRight(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// 오른쪽 찾기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 오른쪽 벡터 구하기
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 해당 방향으로 이동 추가
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

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	const float AttackSpeedRate = 1.0f;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(BasicComboMontage, AttackSpeedRate);

		FOnMontageEnded EndDelegate;

		EndDelegate.BindUObject(this, &APlayerCharacter::ComboEnd);

		AnimInstance->Montage_SetEndDelegate(EndDelegate, BasicComboMontage);

		ComboTimerHandle.Invalidate();

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
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, BasicComboData->MaxComboCount);

		// 애님 인스턴스 가져오기
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			// 다음 섹션의 이름 만들기
			FName SectionName = *FString::Printf(TEXT("%s%d"), *BasicComboData->SectionPrefix, CurrentComboCount);

			// 다음 섹션으로 이동하기
			AnimInstance->Montage_JumpToSection(SectionName, BasicComboMontage);

			// 타이머 재설정
			SetComboTimer();
			// 콤보 입력 판별 초기화
			bHasComboInput = false;
		}
	}
}

void APlayerCharacter::SetComboTimer()
{
	// 인덱스 조정
	// * 콤보 인덱스 : 1, 2, 3, 4
	// * 배열 인덱스 : 0, 1, 2, 3
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

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::EndSprint);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::RollStart);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::BasicAttack);
}

