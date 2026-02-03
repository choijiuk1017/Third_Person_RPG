// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/WeaponUpgradeEntryWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/ListView.h"
#include "Components/ListViewBase.h"

#include "Third_Person_RPG/Inventory/WeaponListItemObject.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"

void UWeaponUpgradeEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CachedItemObject = Cast<UWeaponListItemObject>(ListItemObject);
	Refresh();

	if (UListView* ListView = Cast<UListView>(GetOwningListView()))
	{
		const bool bSelectedNow = (ListView->GetSelectedItem() == ListItemObject);
		SetSelectedVisual(bSelectedNow);
	}
	else
	{
		SetSelectedVisual(false);
	}
}

void UWeaponUpgradeEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	SetSelectedVisual(bIsSelected);
}

void UWeaponUpgradeEntryWidget::Refresh()
{
	if (!CachedItemObject) return;

	UInventoryItem* Item = CachedItemObject->Item.Get();

	if (ItemIconImage)
	{
		UTexture2D* Tex = Item ? Item->GetItemTexture() : nullptr;

		if (Tex)
		{
			ItemIconImage->SetBrushFromTexture(Tex, true);
			ItemIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			ItemIconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::FromString(Item ? Item->GetItemName() : TEXT("None")));
	}

	if (EnhanceLevelText)
	{
		const int32 Lv = Item ? Item->EnhanceLevel : 0;
		EnhanceLevelText->SetText(FText::AsNumber(Lv));
	}

	if (NeededCurrencyText)
	{
		NeededCurrencyText->SetText(FText::AsNumber(CachedItemObject->NeededCurrency));
	}

	if (AffordableText)
	{
		AffordableText->SetText(CachedItemObject->bAffordable
			? FText::FromString(TEXT("OK"))
			: FText::FromString(TEXT("NO")));
	}
}

void UWeaponUpgradeEntryWidget::SetSelectedVisual(bool bSelected)
{
	if (SelectionBorder)
	{
		SelectionBorder->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

UObject* UWeaponUpgradeEntryWidget::GetMyListItemObject() const
{
	return CachedItemObject.Get();
}