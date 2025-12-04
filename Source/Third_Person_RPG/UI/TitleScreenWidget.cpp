// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/TitleScreenWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UTitleScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;

	UpdateSelection();

	bIsFocusable = true;

	SetKeyboardFocus();

	if (StartAnim)
	{
		PlayAnimation(StartAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UTitleScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UTitleScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Down)
	{
		CurrentIndex = (CurrentIndex + 1) % 3;
		UpdateSelection();
		return FReply::Handled();
	}

	if (Key == EKeys::Up)
	{
		CurrentIndex = (CurrentIndex - 1 + 3) % 3;
		UpdateSelection();
		return FReply::Handled();
	}

	if (Key == EKeys::Enter || Key == EKeys::SpaceBar)
	{
		ExecuteSelection();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UTitleScreenWidget::UpdateSelection()
{
	FSlateColor NormalColor = FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f));

	FSlateColor SelectedColor = FSlateColor(FLinearColor(1.0f, 0.8f, 0.3f));

	NewGameText->SetColorAndOpacity(NormalColor);
	LoadGameText->SetColorAndOpacity(NormalColor);
	ExitText->SetColorAndOpacity(NormalColor);

	switch (CurrentIndex)
	{
	case 0: NewGameText->SetColorAndOpacity(SelectedColor); break;
	case 1: LoadGameText->SetColorAndOpacity(SelectedColor); break;
	case 2: ExitText->SetColorAndOpacity(SelectedColor); break;
	}
}

void UTitleScreenWidget::ExecuteSelection()
{
	switch (CurrentIndex)
	{
	case 0:
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Dungeon1"));
		break;

	case 1:
		break;

	case 2:
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
		break;
	}
}