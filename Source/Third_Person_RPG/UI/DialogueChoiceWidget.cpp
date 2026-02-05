// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/DialogueChoiceWidget.h"

#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

UDialogueChoiceWidget::UDialogueChoiceWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UDialogueChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetSelectedIndex(0);
}

void UDialogueChoiceWidget::SetTexts(const FText& InQuestion, const FText& InYes, const FText& InNo)
{
	if (QuestionText) QuestionText->SetText(InQuestion);
	if (YesText) YesText->SetText(InYes);
	if (NoText) NoText->SetText(InNo);
}

void UDialogueChoiceWidget::SetSelectedIndex(int32 Index)
{
	SelectedIndex = FMath::Clamp(Index, 0, 1);

	const bool bYes = (SelectedIndex == 0);

	if (YesHighlight) YesHighlight->SetVisibility(bYes ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	if (NoHighlight)  NoHighlight->SetVisibility(!bYes ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

FReply UDialogueChoiceWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Left)
	{
		SetSelectedIndex(0);
		return FReply::Handled();
	}
	if (Key == EKeys::Right)
	{
		SetSelectedIndex(1);
		return FReply::Handled();
	}

	if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
	{
		const bool bYes = (SelectedIndex == 0);
		OnChoiceConfirmed.Broadcast(bYes);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}