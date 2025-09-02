// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/CurrentEquipedWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCurrentEquipedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ÃÊ±â°ª: ºóÄ­À¸·Î
	UpdateWeaponIcon(nullptr);
	UpdatePotion(nullptr, 0);
}

void UCurrentEquipedWidget::SetImageFromTexture(UImage* Target, UTexture2D* Texture)
{
	if (!Target) return;

	if (Texture)
	{
		Target->SetVisibility(ESlateVisibility::Visible);
		Target->SetBrushFromTexture(Texture, /*bMatchSize=*/true);
	}
	else if (EmptySlotTexture)
	{
		Target->SetVisibility(ESlateVisibility::Visible);
		Target->SetBrushFromTexture(EmptySlotTexture, true);
	}
	else
	{
		// ºó ÅØ½ºÃ³µµ ¾øÀ¸¸é ±×³É ¼û±è
		Target->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCurrentEquipedWidget::SetCountText(int32 Count)
{
	if (!PotionCountText) return;

	if (Count > 0)
	{
		PotionCountText->SetVisibility(ESlateVisibility::HitTestInvisible);
		PotionCountText->SetText(FText::AsNumber(Count));
	}
	else
	{
		PotionCountText->SetVisibility(ESlateVisibility::Hidden);
		PotionCountText->SetText(FText::GetEmpty());
	}
}

void UCurrentEquipedWidget::UpdateWeaponIcon(UTexture2D* InIcon)
{
	SetImageFromTexture(WeaponIcon, InIcon);
}

void UCurrentEquipedWidget::UpdatePotion(UTexture2D* InIcon, int32 InCount)
{
	SetImageFromTexture(PotionIcon, InIcon);
	SetCountText(InCount);
}

void UCurrentEquipedWidget::ClearWeapon()
{
	UpdateWeaponIcon(nullptr);
}

void UCurrentEquipedWidget::ClearPotion()
{
	UpdatePotion(nullptr, 0);
}
