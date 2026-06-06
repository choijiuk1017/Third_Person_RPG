// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerSavePointComponent.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "Third_Person_RPG/UI/SavePointUI/SavePointMenu.h"
#include "Third_Person_RPG/UI/CurrentEquipedWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

UPlayerSavePointComponent::UPlayerSavePointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerSavePointComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerPlayer)
	{
		OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
	}
}

void UPlayerSavePointComponent::InitializeSavePointComponent(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
}

void UPlayerSavePointComponent::SetOverlappingSavePoint(ASavePoint* SavePoint)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->OverlappingSavePoint = SavePoint;
}

void UPlayerSavePointComponent::ReSetOverlappingSavePoint()
{
	if (!OwnerPlayer) return;

	OwnerPlayer->OverlappingSavePoint = nullptr;
}

void UPlayerSavePointComponent::InteractingSavePoint(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerPlayer) return;

	UE_LOG(LogTemp, Warning, TEXT("세이브 포인트"));

	if (!OwnerPlayer->OverlappingSavePoint) return;

	OwnerPlayer->bIsInteracting = false;

	OwnerPlayer->OverlappingSavePoint->SavePointInfo.bIsDiscovered = true;

	UTPRGameInstance* GI =
		Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(OwnerPlayer->GetWorld()));

	if (GI)
	{
		GI->RegisterSavePoint(OwnerPlayer->OverlappingSavePoint->SavePointInfo);
		GI->LastRestedSavePointID = OwnerPlayer->OverlappingSavePoint->SavePointInfo.SavePointID;
		GI->SaveGameData();
	}

	OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	OwnerPlayer->HPPotionCount = OwnerPlayer->MaxHPPotionCount;
	OwnerPlayer->FPPotionCount = OwnerPlayer->MaxFPPotionCount;

	OwnerPlayer->CombatStats.CurrentHP = OwnerPlayer->DerivedStats.MaxHP;
	OwnerPlayer->CombatStats.CurrentFP = OwnerPlayer->DerivedStats.MaxFP;
	OwnerPlayer->CombatStats.CurrentStamina = OwnerPlayer->DerivedStats.MaxStamina;

	OwnerPlayer->OnHPChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentHP,
		OwnerPlayer->DerivedStats.MaxHP
	);

	OwnerPlayer->OnFPChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentFP,
		OwnerPlayer->DerivedStats.MaxFP
	);

	OwnerPlayer->OnStaminaChanged.Broadcast(
		OwnerPlayer->CombatStats.CurrentStamina,
		OwnerPlayer->DerivedStats.MaxStamina
	);

	if (!OwnerPlayer->SavePointMenuInstance && OwnerPlayer->SavePointMenuClass)
	{
		OwnerPlayer->SavePointMenuInstance =
			CreateWidget<USavePointMenu>(OwnerPlayer->GetWorld(), OwnerPlayer->SavePointMenuClass);

		if (OwnerPlayer->SavePointMenuInstance)
		{
			OwnerPlayer->SavePointMenuInstance->OwningActor = OwnerPlayer;
			OwnerPlayer->SavePointMenuInstance->AddToViewport();

			APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
			if (PC)
			{
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(OwnerPlayer->SavePointMenuInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;

				PC->SetIgnoreMoveInput(true);
				PC->SetIgnoreLookInput(true);
			}

			if (!OwnerPlayer->bPausedBySavePoint)
			{
				OwnerPlayer->SavedGlobalTimeDilation =
					UGameplayStatics::GetGlobalTimeDilation(OwnerPlayer->GetWorld());

				UGameplayStatics::SetGlobalTimeDilation(OwnerPlayer->GetWorld(), 0.0f);

				OwnerPlayer->bPausedBySavePoint = true;
			}
		}
	}
}

void UPlayerSavePointComponent::EndInteractSavePoint()
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->bIsKneeling && OwnerPlayer->OverlappingSavePoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("세이브 포인트 상호작용 종료"));

		OwnerPlayer->bIsKneeling = false;
		OwnerPlayer->bIsInteracting = false;

		if (OwnerPlayer->SavePointMenuInstance)
		{
			if (OwnerPlayer->SavePointMenuInstance->IsInViewport())
			{
				OwnerPlayer->SavePointMenuInstance->RemoveFromParent();
			}

			OwnerPlayer->SavePointMenuInstance = nullptr;
		}

		if (OwnerPlayer->bPausedBySavePoint)
		{
			if (!OwnerPlayer->bPausedByInventory)
			{
				UGameplayStatics::SetGlobalTimeDilation(
					OwnerPlayer->GetWorld(),
					OwnerPlayer->SavedGlobalTimeDilation
				);
			}

			OwnerPlayer->bPausedBySavePoint = false;
		}

		OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		if (UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance())
		{
			AnimInstance->StopAllMontages(0.2f);
		}

		if (OwnerPlayer->CurrentWeapon)
		{
			OwnerPlayer->CurrentWeapon->SetActorHiddenInGame(false);
			OwnerPlayer->CurrentWeapon->SetActorEnableCollision(true);
		}

		if (OwnerPlayer->CurrentEquipedWidgetInstance)
		{
			OwnerPlayer->CurrentEquipedWidgetInstance->UpdatePotionCounts(
				OwnerPlayer->HPPotionCount,
				OwnerPlayer->FPPotionCount
			);
		}

		APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
		if (PC)
		{
			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);
		}

		OwnerPlayer->SetStatusHUDVisible(true);
	}
}

