// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/NPC/BlackSmith.h"

#include "Blueprint/UserWidget.h" 
#include "GameFramework/PlayerController.h" 

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/UI/WeaponUpgradeWidget.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"

void ABlackSmith::OnTalkFinished(APlayerCharacter* Player)
{
	if (!Player) return;
	if (bUpgradeUIOpened) return;
	if (!UpgradeWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC) return;

	UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>();
	if (!InvComp) return;

	TArray<UInventoryItem*> WeaponItems;
	{
		const TArray<UInventoryItem*>& Equip = InvComp->GetItemsByType(EItemType::IT_Weapon);
		for (UInventoryItem* Item : Equip)
		{
			if (IsValid(Item))
			{
				WeaponItems.Add(Item);
			}
		}
	}

	UpgradeWidgetInstance = CreateWidget<UWeaponUpgradeWidget>(PC, UpgradeWidgetClass);
	if (!UpgradeWidgetInstance) return;

	UpgradeWidgetInstance->AddToViewport();
	bUpgradeUIOpened = true;

	UpgradeWidgetInstance->OnClosed.AddDynamic(this, &ABlackSmith::OnUpgradeWidgetClosed);

	UpgradeWidgetInstance->InitUpgradeUI(Player, WeaponItems);
}

void ABlackSmith::OnUpgradeWidgetClosed()
{
	bUpgradeUIOpened = false;
	UpgradeWidgetInstance = nullptr;
}