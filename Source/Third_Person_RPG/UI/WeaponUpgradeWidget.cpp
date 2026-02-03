// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/WeaponUpgradeWidget.h"

#include "Components/ListView.h"
#include "Components/ListViewBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Input/Reply.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Inventory/WeaponListItemObject.h"
#include "Third_Person_RPG/UI/WeaponUpgradeEntryWidget.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "InputCoreTypes.h" 


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
		UpdateHighlightFromSelection();
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

		if (WeaponListView)
		{
			WeaponListView->SetKeyboardFocus();
		}
		else
		{
			SetKeyboardFocus();
		}
	}
}

FReply UWeaponUpgradeWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Down || Key == EKeys::S)
	{
		if (MoveListSelection(+1)) return FReply::Handled();
	}
	else if (Key == EKeys::Up || Key == EKeys::W)
	{
		if (MoveListSelection(-1)) return FReply::Handled();
	}

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
			UpdateHighlightFromSelection();
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
	UpdateHighlightFromSelection();
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

bool UWeaponUpgradeWidget::MoveListSelection(int32 Delta)
{
	if (!WeaponListView) return false;

	const int32 Num = ListItems.Num();
	if (Num <= 0) return false;

	UObject* CurSelected = WeaponListView->GetSelectedItem();
	int32 CurIndex = 0;

	if (CurSelected)
	{
		for (int32 i = 0; i < Num; ++i)
		{
			if (ListItems[i] == CurSelected)
			{
				CurIndex = i;
				break;
			}
		}
	}

	int32 NewIndex = CurIndex + Delta;
	NewIndex = FMath::Clamp(NewIndex, 0, Num - 1);

	if (NewIndex == CurIndex && CurSelected) return false;

	UObject* NewItem = ListItems[NewIndex].Get();
	if (!NewItem) return false;

	WeaponListView->SetSelectedItem(NewItem);
	WeaponListView->ScrollIndexIntoView(NewIndex);

	WeaponListView->SetKeyboardFocus();
	return true;
}

void UWeaponUpgradeWidget::UpdateHighlightFromSelection()
{
	if (!WeaponListView) return;

	UObject* Selected = WeaponListView->GetSelectedItem();

	const TArray<UUserWidget*>& Entries = WeaponListView->GetDisplayedEntryWidgets();
	for (UUserWidget* W : Entries)
	{
		UWeaponUpgradeEntryWidget* Entry = Cast<UWeaponUpgradeEntryWidget>(W);
		if (!Entry) continue;

		const bool bSelected = (Entry->GetMyListItemObject() == Selected);
		Entry->SetSelectedVisual(bSelected);
	}
}

