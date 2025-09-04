// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/CurrentEquipedWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCurrentEquipedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ÃÊ±â°ª: ºóÄ­À¸·Î

	bIsHPPotion = true;

	UpdateWeaponIcon(nullptr);
	UpdatePotion( 0);
}

void UCurrentEquipedWidget::SetImageFromTexture(UImage* Target, UTexture2D* Texture)
{
	if (!Target) return;

	if (Texture)
	{
		Target->SetVisibility(ESlateVisibility::Visible);
		Target->SetBrushFromTexture(Texture, true);
	}
	else
	{
		Target->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCurrentEquipedWidget::SetPotionImageFromTexture(UImage* Target)
{
	if (!Target) return;

	if (bIsHPPotion)
	{
		Target->SetVisibility(ESlateVisibility::Visible);
		Target->SetBrushFromTexture(HPPotionTexture, true);

		SubPotionIcon->SetBrushFromTexture(FPPotionTexture, true);
	}
	else
	{
		Target->SetVisibility(ESlateVisibility::Visible);
		Target->SetBrushFromTexture(FPPotionTexture, true);

		SubPotionIcon->SetBrushFromTexture(HPPotionTexture, true);
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
}

void UCurrentEquipedWidget::UpdateWeaponIcon(UTexture2D* InIcon)
{
	SetImageFromTexture(WeaponIcon, InIcon);
}

void UCurrentEquipedWidget::UpdatePotion(int32 InCount)
{
	SetPotionImageFromTexture(PotionIcon);
	SetCountText(InCount);
}

void UCurrentEquipedWidget::ChangePotion(uint8 bIsHPPoiton, int32 Count)
{
	bIsHPPotion = bIsHPPoiton;

	UpdatePotion(Count);
}

void UCurrentEquipedWidget::ClearWeapon()
{
	UpdateWeaponIcon(nullptr);
}

void UCurrentEquipedWidget::ClearPotion()
{
	UpdatePotion(0);
}
