// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/WeaponUpgradeWidget.h"

#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Input/Reply.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Inventory/WeaponListItemObject.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"

void UWeaponUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (WeaponListView)
	{
		WeaponListView->OnItemSelectionChanged().AddUObject(this, &UWeaponUpgradeWidget::OnWeaponSelectionChanged);
	}

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UWeaponUpgradeWidget::OnUpgradeClicked);
	}
}

void UWeaponUpgradeWidget::InitUpgradeUI(APlayerCharacter* InPlayer, const TArray<UInventoryItem*>& InWeapons)
{
	Player = InPlayer;
	if (!Player) return;

	CachedInvComp = Player->FindComponentByClass<UInventoryComponent>();

	// 인벤토리 변경 이벤트 바인딩(중복 방지하려면 RemoveDynamic 후 AddDynamic 권장)
	if (CachedInvComp)
	{
		CachedInvComp->OnInventoryChanged.AddDynamic(this, &UWeaponUpgradeWidget::RefreshFromInventory);
	}

	RebuildList(InWeapons);

	// 첫 항목 자동 선택
	if (WeaponListView && ListItems.Num() > 0)
	{
		WeaponListView->SetSelectedItem(ListItems[0]);
	}

	RefreshRightPanel();

	// 입력 모드/UI 포커스
	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
		PC->bShowMouseCursor = true;

		SetKeyboardFocus();
	}
}

FReply UWeaponUpgradeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
	{
		OnUpgradeClicked();
		return FReply::Handled();
	}

	if (Key == EKeys::X)
	{
		CloseWidget();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWeaponUpgradeWidget::CloseWidget()
{
	if (APlayerController* PC = Player ? Cast<APlayerController>(Player->GetController()) : nullptr)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}

	OnClosed.Broadcast();

	RemoveFromParent();
}


void UWeaponUpgradeWidget::RefreshFromInventory()
{
	if (!CachedInvComp) return;

	TArray<UInventoryItem*> Weapons;
	const TArray<UInventoryItem*>& Equip = CachedInvComp->GetItemsByType(EItemType::IT_Weapon);
	for (UInventoryItem* Item : Equip)
	{
		if (IsValid(Item))
		{
			Weapons.Add(Item);
		}
	}

	RebuildList(Weapons);

	if (WeaponListView)
	{
		UObject* NewSelect = nullptr;

		UInventoryItem* PrevItem = SelectedItemObject ? SelectedItemObject->Item.Get() : nullptr;
		if (PrevItem)
		{
			for (UWeaponListItemObject* Obj : ListItems)
			{
				if (Obj && Obj->Item.Get() == PrevItem)
				{
					NewSelect = Obj;
					break;
				}
			}
		}

		if (!NewSelect && ListItems.Num() > 0)
		{
			NewSelect = ListItems[0];
		}

		if (NewSelect)
		{
			WeaponListView->SetSelectedItem(NewSelect);
		}
	}

	RefreshRightPanel();
}


void UWeaponUpgradeWidget::RebuildList(const TArray<UInventoryItem*>& Weapons)
{
	ListItems.Empty();

	const int32 Owned = Player ? Player->GetCurrency() : 0;

	for (UInventoryItem* Item : Weapons)
	{
		if (!IsValid(Item)) continue;

		UWeaponListItemObject* Obj = NewObject<UWeaponListItemObject>(this);
		Obj->Item = Item;

		const int32 Need = CalcNeededCurrency(Item);
		Obj->NeededCurrency = Need;
		Obj->bAffordable = (Owned >= Need);

		ListItems.Add(Obj);
	}

	if (WeaponListView)
	{
		WeaponListView->ClearListItems();
		for (UWeaponListItemObject* Obj : ListItems)
		{
			WeaponListView->AddItem(Obj);
		}
	}
}

void UWeaponUpgradeWidget::OnWeaponSelectionChanged(UObject* SelectedItem)
{
	SelectedItemObject = Cast<UWeaponListItemObject>(SelectedItem);
	RefreshRightPanel();
}

int32 UWeaponUpgradeWidget::CalcNeededCurrency(const UInventoryItem* Item) const
{
	
	if (!Item) return 0;
	return 200 + (Item->EnhanceLevel * 100);
}

bool UWeaponUpgradeWidget::CanUpgrade(const UInventoryItem* Item, int32& OutNeededCurrency) const
{
	OutNeededCurrency = 0;
	if (!Player || !Item) return false;

	OutNeededCurrency = CalcNeededCurrency(Item);

	const int32 Owned = Player->GetCurrency();
	return Owned >= OutNeededCurrency;
}

void UWeaponUpgradeWidget::DoUpgrade(UInventoryItem* Item, int32 NeededCurrency)
{
	if (!Player || !Item) return;

	if (!Player->SpendCurrency(NeededCurrency))
	{
		return;
	}

	Item->EnhanceLevel += 1;

	if (CachedInvComp)
	{
		CachedInvComp->OnInventoryChanged.Broadcast();
	}

	if (Player && Player->GetCurrentWeapon()) 
	{
		Player->ApplyWeaponStats(Player->GetCurrentWeapon());
	}

	if (Player)
	{
		if (UInventoryComponent* Inv = Player->FindComponentByClass<UInventoryComponent>())
		{
			Inv->OnInventoryChanged.Broadcast();
		}
	}
}

void UWeaponUpgradeWidget::RefreshRightPanel()
{
	const int32 Owned = Player ? Player->GetCurrency() : 0;

	if (OwnedCurrencyText)
	{
		OwnedCurrencyText->SetText(FText::AsNumber(Owned));
	}

	UInventoryItem* Item = (SelectedItemObject ? SelectedItemObject->Item.Get() : nullptr);

	int32 Needed = 0;
	const bool bCan = CanUpgrade(Item, Needed);

	if (NeededCurrencyText)
	{
		NeededCurrencyText->SetText(FText::AsNumber(Needed));
	}

	if (UpgradeButton)
	{
		UpgradeButton->SetIsEnabled(bCan);
	}
}

void UWeaponUpgradeWidget::OnUpgradeClicked()
{
	UInventoryItem* Item = (SelectedItemObject ? SelectedItemObject->Item.Get() : nullptr);
	if (!Item) return;

	int32 Needed = 0;
	if (!CanUpgrade(Item, Needed))
	{
		return;
	}
	DoUpgrade(Item, Needed);
}