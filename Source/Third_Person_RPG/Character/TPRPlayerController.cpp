// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/TPRPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ATPRPlayerController::ATPRPlayerController()
{
	bShowMouseCursor = false;


}

void ATPRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<APlayerCharacter>(GetPawn());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (IMC_Basic)
		{
			Subsystem->AddMappingContext(IMC_Basic, 0);
		}
	}
}

void ATPRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CachedCharacter = Cast<APlayerCharacter>(InPawn);
}


void ATPRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent);

	EIC->BindAction(IA_BasicMove, ETriggerEvent::Triggered, this, &ATPRPlayerController::Input_Move);
	EIC->BindAction(IA_BasicLook, ETriggerEvent::Triggered, this, &ATPRPlayerController::Input_Look);
	EIC->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &ATPRPlayerController::Input_Sprint_Start);
	EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ATPRPlayerController::Input_Sprint_End);
	EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ATPRPlayerController::Input_Attack);
	EIC->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &ATPRPlayerController::Input_Roll);
	EIC->BindAction(IA_DrinkPotion, ETriggerEvent::Started, this, &ATPRPlayerController::Input_DrinkPotion);
	EIC->BindAction(IA_ChangePotion, ETriggerEvent::Started, this, &ATPRPlayerController::Input_ChangePotion);
	EIC->BindAction(IA_Skill, ETriggerEvent::Started, this, &ATPRPlayerController::Input_Skill);
	EIC->BindAction(IA_Interaction, ETriggerEvent::Started, this, &ATPRPlayerController::Input_Interaction);
	EIC->BindAction(IA_Inventory, ETriggerEvent::Started, this, &ATPRPlayerController::Input_Inventory);
	EIC->BindAction(IA_Enter, ETriggerEvent::Started, this, &ATPRPlayerController::Input_Enter);
}


void ATPRPlayerController::Input_Move(const FInputActionValue& Value)
{
	if (!CachedCharacter || CachedCharacter->bIsDead) return;

	const FVector2D Movement = Value.Get<FVector2D>();
	const FRotator ControlRot = GetControlRotation();
	const FRotator YawRot(0, ControlRot.Yaw, 0);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	CachedCharacter->AddMovementInput(Forward, Movement.X);
	CachedCharacter->AddMovementInput(Right, Movement.Y);
}

void ATPRPlayerController::Input_Look(const FInputActionValue& Value)
{
	const FVector2D Look = Value.Get<FVector2D>();
	AddYawInput(Look.X);
	AddPitchInput(Look.Y);
}

void ATPRPlayerController::Input_Sprint_Start()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->BeginSprint();
}

void ATPRPlayerController::Input_Sprint_End()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->EndSprint();
}

void ATPRPlayerController::Input_Attack()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->BasicAttack();
}

void ATPRPlayerController::Input_Roll()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->RollStart();
}

void ATPRPlayerController::Input_Skill()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->SkillStart();
}


void ATPRPlayerController::Input_Interaction()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->Interact();
}

void ATPRPlayerController::Input_Inventory()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->ToggleInventory();
}

void ATPRPlayerController::Input_DrinkPotion()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->DrinkPotion();
}

void ATPRPlayerController::Input_ChangePotion()
{
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->ChangePotion();
}

void ATPRPlayerController::Input_Enter()
{
	if (!IA_Enter)
	{
		UE_LOG(LogTemp, Error, TEXT("[PC] IA_Enter is NULL"));
		return;
	}
	if (CachedCharacter || !CachedCharacter->bIsDead) CachedCharacter->OnAdvanceDialogue();
}