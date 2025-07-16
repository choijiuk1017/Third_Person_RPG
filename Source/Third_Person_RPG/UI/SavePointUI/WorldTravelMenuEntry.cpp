// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/WorldTravelMenuEntry.h"
#include "Components/TextBlock.h"

void UWorldTravelMenuEntry::SetPointName(const FString& NewName)
{
	PointName = NewName;

	if (Text_PointName)
	{
		Text_PointName->SetText(FText::FromString(PointName));
	}
}

void UWorldTravelMenuEntry::SetSelected(bool bSelected)
{
	if (Text_PointName)
	{
		FSlateColor Color = bSelected ? FSlateColor(FLinearColor::Yellow) : FSlateColor(FLinearColor::White);
		Text_PointName->SetColorAndOpacity(Color);
	}
}