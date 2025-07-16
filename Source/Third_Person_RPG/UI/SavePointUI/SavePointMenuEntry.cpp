// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/SavePointMenuEntry.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void USavePointMenuEntry::NativeConstruct()
{
	Super::NativeConstruct();

	//// 시작 시 선택 강조 끄기
	//if (Border_Highlight)
	//{
	//	Border_Highlight->SetVisibility(ESlateVisibility::Collapsed);
	//}
}


void USavePointMenuEntry::SetMenuText(const FText& InText)
{
	if (Text_MenuName)
	{
		Text_MenuName->SetText(InText);
	}
}

void USavePointMenuEntry::SetSelected(bool bSelected)
{
	if (Border_Highlight)
	{
		Border_Highlight->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}