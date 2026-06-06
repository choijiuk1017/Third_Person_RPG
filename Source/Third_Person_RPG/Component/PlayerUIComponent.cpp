// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerUIComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/UI/InventoryUI/InventoryWidget.h"
#include "Third_Person_RPG/UI/InteractionWidget.h"
#include "Third_Person_RPG/UI/PlayerStatusWidget.h"
#include "Third_Person_RPG/UI/CurrencyWidget.h"
#include "Third_Person_RPG/UI/CurrentEquipedWidget.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UPlayerUIComponent::UPlayerUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerPlayer)
	{
		OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
	}
}

void UPlayerUIComponent::InitializeUIComponent(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
}

void UPlayerUIComponent::ShowInteractionUI(const FText& InText)
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->InteractionWidgetInstance)
	{
		OwnerPlayer->InteractionWidgetInstance->SetHelpText(InText.ToString());
		OwnerPlayer->InteractionWidgetInstance->SetIsEnabled(true);
		OwnerPlayer->InteractionWidgetInstance->SetRenderOpacity(1.0f);
		OwnerPlayer->InteractionWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPlayerUIComponent::HideInteractionUI()
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->InteractionWidgetInstance)
	{
		OwnerPlayer->InteractionWidgetInstance->SetIsEnabled(false);
		OwnerPlayer->InteractionWidgetInstance->SetRenderOpacity(0.0f);
		OwnerPlayer->InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerUIComponent::SetStatusHUDVisible(bool bVisible)
{
	if (!OwnerPlayer) return;

	if (!OwnerPlayer->PlayerStatusWidgetInstance) return;
	if (!OwnerPlayer->CurrentEquipedWidgetInstance) return;
	if (!OwnerPlayer->CurrencyWidgetInstance) return;

	if (bVisible)
	{
		OwnerPlayer->PlayerStatusWidgetInstance->SetVisibility(OwnerPlayer->StatusHUDSavedVisibility);
		OwnerPlayer->CurrentEquipedWidgetInstance->SetVisibility(OwnerPlayer->StatusHUDSavedVisibility);
		OwnerPlayer->CurrencyWidgetInstance->SetVisibility(OwnerPlayer->StatusHUDSavedVisibility);
	}
	else
	{
		OwnerPlayer->PlayerStatusWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		OwnerPlayer->CurrentEquipedWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		OwnerPlayer->CurrencyWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerUIComponent::ToggleInventory()
{
	if (!OwnerPlayer) return;
	if (OwnerPlayer->bIsDead) return;

	if (OwnerPlayer->bIsPopupInventory)
	{
		CloseInventory();
	}
	else
	{
		PopUpInventory();
	}
}

void UPlayerUIComponent::PopUpInventory()
{
	if (!OwnerPlayer) return;

	if (!OwnerPlayer->bPausedByInventory)
	{
		OwnerPlayer->SavedGlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(OwnerPlayer->GetWorld());
		UGameplayStatics::SetGlobalTimeDilation(OwnerPlayer->GetWorld(), 0.0f);
		OwnerPlayer->bPausedByInventory = true;
	}

	if (!OwnerPlayer->InventoryWidgetInstance && OwnerPlayer->InventoryWidgetClass)
	{
		OwnerPlayer->InventoryWidgetInstance =
			CreateWidget<UInventoryWidget>(OwnerPlayer->GetWorld(), OwnerPlayer->InventoryWidgetClass);

		if (OwnerPlayer->InventoryWidgetInstance)
		{
			OwnerPlayer->InventoryWidgetInstance->OwningActor = OwnerPlayer;
			OwnerPlayer->InventoryWidgetInstance->AddToViewport();
		}
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC && OwnerPlayer->InventoryWidgetInstance)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(OwnerPlayer->InventoryWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;

		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}

	SetStatusHUDVisible(false);

	OwnerPlayer->bIsPopupInventory = true;
}

void UPlayerUIComponent::CloseInventory()
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->bPausedByInventory)
	{
		if (!OwnerPlayer->bPausedBySavePoint)
		{
			UGameplayStatics::SetGlobalTimeDilation(
				OwnerPlayer->GetWorld(),
				OwnerPlayer->SavedGlobalTimeDilation
			);
		}

		OwnerPlayer->bPausedByInventory = false;
	}

	if (OwnerPlayer->InventoryWidgetInstance)
	{
		OwnerPlayer->InventoryWidgetInstance->RemoveFromParent();
		OwnerPlayer->InventoryWidgetInstance = nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;

		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
	}

	SetStatusHUDVisible(true);

	OwnerPlayer->bIsPopupInventory = false;
}

void UPlayerUIComponent::ShowWeaponInfo(UWeaponItemData* WeaponData)
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->InventoryWidgetInstance && WeaponData)
	{
		OwnerPlayer->InventoryWidgetInstance->UpdateWeaponInfo(WeaponData);
	}
}

void UPlayerUIComponent::RefreshCurrentEquipped_Weapon(UTexture2D* WeaponIconTexture)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CurrentWeaponIcon = WeaponIconTexture;

	if (OwnerPlayer->CurrentEquipedWidgetInstance)
	{
		OwnerPlayer->CurrentEquipedWidgetInstance->UpdateWeaponIcon(WeaponIconTexture);
	}
}

void UPlayerUIComponent::RefreshCurrentEquipped_Potion(UTexture2D* PotionIconTexture, int32 NewCount)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->HPPotionCount = NewCount;

	if (OwnerPlayer->CurrentEquipedWidgetInstance)
	{
		OwnerPlayer->CurrentEquipedWidgetInstance->UpdatePotion(NewCount);
	}
}

