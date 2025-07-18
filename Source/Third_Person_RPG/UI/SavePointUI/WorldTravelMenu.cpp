// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/WorldTravelMenu.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Third_Person_RPG/UI/SavePointUI/WorldTravelMenuEntry.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"

UWorldTravelMenu::UWorldTravelMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UWorldTravelMenu::NativeConstruct()
{
	Super::NativeConstruct();

	this->SetKeyboardFocus();
	FSlateApplication::Get().SetKeyboardFocus(this->TakeWidget(), EFocusCause::SetDirectly);

	if (const UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		// 구조체 기반으로 변경
		TArray<FSavePointInfo> SavePoints;
		GI->GetSavePointMap().GenerateValueArray(SavePoints);
		SetTravelPoints(SavePoints);
	}

	SelectedIndex = 0;
	UpdateSelectionVisual();
}

FReply UWorldTravelMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey PressedKey = InKeyEvent.GetKey();

	if (PressedKey == EKeys::W || PressedKey == EKeys::Up)
	{
		NavigateUp();
		return FReply::Handled();
	}
	else if (PressedKey == EKeys::S || PressedKey == EKeys::Down)
	{
		NavigateDown();
		return FReply::Handled();
	}
	else if (PressedKey == EKeys::Enter)
	{
		ConfirmSelection();
		return FReply::Handled();
	}
	

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}


void UWorldTravelMenu::SetTravelPoints(const TArray<FSavePointInfo>& SavePoints)
{
	if (!TravelEntryClass || !VerticalBox_TravelList) return;

	VerticalBox_TravelList->ClearChildren();
	EntryWidgets.Empty();

	for (const FSavePointInfo& Info : SavePoints)
	{
		UWorldTravelMenuEntry* Entry = CreateWidget<UWorldTravelMenuEntry>(this, TravelEntryClass);
		if (Entry)
		{
			Entry->SetSavePointInfo(Info);
			Entry->OnSelected.BindUObject(this, &UWorldTravelMenu::UpdatePreviewImage);
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

	const FSavePointInfo& Info = EntryWidgets[SelectedIndex]->GetSavePointInfo();

	if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		GI->SetPendingSavePoint(Info.SavePointID);
	}

	RemoveFromParent();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UInventoryComponent* Inventory = PC->FindComponentByClass<UInventoryComponent>())
		{
			if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				GI->CacheInventory(Inventory->GetAllItems()); // 함수 필요, 아래에 정의함
			}
		}
	}
	if (LoadingScreen)
	{
		LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
		if (LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(9999); // 가장 위에 표시
		}
	}


	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([=, this]()  
		{
			UGameplayStatics::OpenLevel(this, Info.MapName);
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
}

void UWorldTravelMenu::UpdateSelectionVisual()
{
	for (int32 i = 0; i < EntryWidgets.Num(); ++i)
	{
		EntryWidgets[i]->SetSelected(i == SelectedIndex);
	}

	UpdatePreviewImage(EntryWidgets.IsValidIndex(SelectedIndex)
		? EntryWidgets[SelectedIndex]->GetSavePointInfo().SavePointID.ToString()
		: TEXT(""));
}

void UWorldTravelMenu::UpdatePreviewImage(const FString& PointName)
{
	if (!PreviewImage) return;

	for (UWorldTravelMenuEntry* Entry : EntryWidgets)
	{
		if (Entry->GetSavePointInfo().SavePointID.ToString() == PointName)
		{
			PreviewImage->SetBrushFromTexture(Entry->GetSavePointInfo().PreviewImage);
			return;
		}
	}

	PreviewImage->SetBrushFromTexture(nullptr);
}