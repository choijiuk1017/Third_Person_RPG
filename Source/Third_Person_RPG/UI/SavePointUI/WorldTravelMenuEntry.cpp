// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/WorldTravelMenuEntry.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UWorldTravelMenuEntry::SetSavePointInfo(const FSavePointInfo& Info)
{
	SavePointInfo = Info;
	if (Text_PointName)
	{
		Text_PointName->SetText(Info.DisplayName);
	}
}

FSavePointInfo UWorldTravelMenuEntry::GetSavePointInfo() const
{
	return SavePointInfo;
}

void UWorldTravelMenuEntry::SetSelected(bool bSelected)
{
	if (Border_Highlight)
	{
		Border_Highlight->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}