// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointMenu.h"
#include "Components/VerticalBox.h"
#include "Third_Person_RPG/UI/SavePointMenuEntry.h"

void USavePointMenu::NativeConstruct()
{
	Super::NativeConstruct();

	//// 초기화
	//EntryWidgets.Empty();

	if (!EntryClass || !VerticalBox_MenuList) return;

	UE_LOG(LogTemp, Error, TEXT("Menu Options Num: %d"), MenuOptions.Num());

	for (int32 i = 0; i < MenuOptions.Num(); ++i)
	{
		USavePointMenuEntry* NewEntry = CreateWidget<USavePointMenuEntry>(this, EntryClass);
		if (!NewEntry)
		{
			UE_LOG(LogTemp, Error, TEXT("NewEntry is null at index %d"), i);
			continue;
		}

		NewEntry->SetMenuText(MenuOptions[i]);
		VerticalBox_MenuList->AddChild(NewEntry);
		EntryWidgets.Add(NewEntry);

		UE_LOG(LogTemp, Warning, TEXT("Created Entry %d with text: %s"), i, *MenuOptions[i].ToString());

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(NewEntry, [=]()
			{
				if (!NewEntry) return;

				FGeometry Geo = NewEntry->GetCachedGeometry();
				FVector2D AbsPos = Geo.GetAbsolutePosition();
				FVector2D Size = Geo.GetLocalSize();

				UE_LOG(LogTemp, Warning, TEXT("Entry[%d] Pos: (%.1f, %.1f), Size: (%.1f, %.1f)"),
					i, AbsPos.X, AbsPos.Y, Size.X, Size.Y);

			}), 0.f, false);
	}

	UpdateSelectionVisual(); 
}

void USavePointMenu::MoveSelectionUp()
{
	if (EntryWidgets.Num() == 0) return;

	SelectedIndex = (SelectedIndex - 1 + EntryWidgets.Num()) % EntryWidgets.Num();
	UpdateSelectionVisual();
}

void USavePointMenu::MoveSelectionDown()
{
	if (EntryWidgets.Num() == 0) return;

	SelectedIndex = (SelectedIndex + 1) % EntryWidgets.Num();
	UpdateSelectionVisual();
}

void USavePointMenu::UpdateSelectionVisual()
{
	for (int32 i = 0; i < EntryWidgets.Num(); ++i)
	{
		if (EntryWidgets[i])
		{
			const bool bIsSelected = (i == SelectedIndex);
			EntryWidgets[i]->SetSelected(bIsSelected);
		}
	}
}

void USavePointMenu::ConfirmSelection()
{
	if (!EntryWidgets.IsValidIndex(SelectedIndex)) return;

	const FString& SelectedText = MenuOptions[SelectedIndex].ToString();

	if (SelectedText == TEXT("Leave"))
	{
		// UI 닫고 플레이어 제어권 복원
		RemoveFromParent();
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
	else if (SelectedText == TEXT("Level Up"))
	{
		// 레벨업 창 띄우기 등등
	}
}