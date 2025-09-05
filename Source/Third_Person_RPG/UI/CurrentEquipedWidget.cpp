// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/CurrentEquipedWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCurrentEquipedWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기값: 빈칸으로

	bIsHPPotion = 1;
	HPPotionCount = 0;
	FPPotionCount = 0;

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

void UCurrentEquipedWidget::ApplyPotionIcons()
{
	if (!PotionIcon || !SubPotionIcon) return;

	// 메인/서브 타입 및 개수 계산
	const bool  bMainIsHP = (bIsHPPotion != 0);
	const bool  bSubIsHP = !bMainIsHP;

	const int32 MainCount = bMainIsHP ? HPPotionCount : FPPotionCount;
	const int32 SubCount = bSubIsHP ? HPPotionCount : FPPotionCount;

	UTexture2D* MainTypeTex = bMainIsHP ? HPPotionTexture.Get() : FPPotionTexture.Get();
	UTexture2D* SubTypeTex = bSubIsHP ? HPPotionTexture.Get() : FPPotionTexture.Get();

	UTexture2D* MainToUse = (MainCount > 0) ? MainTypeTex : EmptyPotionTexture.Get();
	UTexture2D* SubToUse = (SubCount > 0) ? SubTypeTex : EmptyPotionTexture.Get();

	SetImageFromTexture(PotionIcon, MainToUse);
	SetImageFromTexture(SubPotionIcon, SubToUse);
}

void UCurrentEquipedWidget::SetCountText(int32 Count)
{
	if (!PotionCountText) return;

	PotionCountText->SetText(FText::AsNumber(FMath::Max(0, Count)));
	PotionCountText->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCurrentEquipedWidget::UpdateWeaponIcon(UTexture2D* InIcon)
{
	SetImageFromTexture(WeaponIcon, InIcon);
}


void UCurrentEquipedWidget::RefreshPotionUI()
{
	ApplyPotionIcons();

	// 현재 메인 타입의 개수 표기
	const int32 MainCount = (bIsHPPotion != 0) ? HPPotionCount : FPPotionCount;
	SetCountText(MainCount);
}

void UCurrentEquipedWidget::UpdatePotion(int32 InCount)
{
	if (bIsHPPotion)
	{
		HPPotionCount = FMath::Max(0, InCount);
	}
	else
	{
		FPPotionCount = FMath::Max(0, InCount);
	}
	RefreshPotionUI();
}


void UCurrentEquipedWidget::UpdatePotionCounts(int32 InHPCount, int32 InFPCount)
{
	HPPotionCount = FMath::Max(0, InHPCount);
	FPPotionCount = FMath::Max(0, InFPCount);

	RefreshPotionUI();
}


void UCurrentEquipedWidget::ChangePotion(uint8 bIsHPPoiton, int32 Count)
{
	bIsHPPotion = bIsHPPoiton;


	if (bIsHPPotion)
	{
		HPPotionCount = FMath::Max(0, Count);
	}
	else
	{
		FPPotionCount = FMath::Max(0, Count);
	}
	RefreshPotionUI();
}

void UCurrentEquipedWidget::ClearWeapon()
{
	UpdateWeaponIcon(nullptr);
}

void UCurrentEquipedWidget::ClearPotion()
{
	HPPotionCount = 0;
	FPPotionCount = 0;
	RefreshPotionUI();
}
