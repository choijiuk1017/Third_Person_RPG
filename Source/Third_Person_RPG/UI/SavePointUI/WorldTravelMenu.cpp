// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/WorldTravelMenu.h"
#include "Components/VerticalBox.h"
#include "Third_Person_RPG/UI/SavePointUI/WorldTravelMenuEntry.h"

void UWorldTravelMenu::NativeConstruct()
{
	Super::NativeConstruct();
	SelectedIndex = 0;
	UpdateSelectionVisual();

}

void UWorldTravelMenu::SetTravelPoints(const TArray<FString>& PointNames)
{
	if (!TravelEntryClass || !VerticalBox_TravelList) return;

	VerticalBox_TravelList->ClearChildren();
	EntryWidgets.Empty();

	for (const FString& Name : PointNames)
	{
		UWorldTravelMenuEntry* Entry = CreateWidget<UWorldTravelMenuEntry>(this, TravelEntryClass);
		if (Entry)
		{
			Entry->SetPointName(Name);
			VerticalBox_TravelList->AddChild(Entry);
			EntryWidgets.Add(Entry);
		}
	}

	SelectedIndex = 0;
	UpdateSelectionVisual();
}

void UWorldTravelMenu::NavigateUp()
{
	if (EntryWidgets.Num() == 0) return;

	SelectedIndex = (SelectedIndex - 1 + EntryWidgets.Num()) % EntryWidgets.Num();
	UpdateSelectionVisual();
}

void UWorldTravelMenu::NavigateDown()
{
	if (EntryWidgets.Num() == 0) return;

	SelectedIndex = (SelectedIndex + 1) % EntryWidgets.Num();
	UpdateSelectionVisual();
}

void UWorldTravelMenu::ConfirmSelection()
{
	if (!EntryWidgets.IsValidIndex(SelectedIndex)) return;

	FString TargetPoint = EntryWidgets[SelectedIndex]->GetPointName();

	UE_LOG(LogTemp, Warning, TEXT("선택한 세이브포인트: %s"), *TargetPoint);

}

void UWorldTravelMenu::UpdateSelectionVisual()
{
	for (int32 i = 0; i < EntryWidgets.Num(); ++i)
	{
		EntryWidgets[i]->SetSelected(i == SelectedIndex);
	}
}