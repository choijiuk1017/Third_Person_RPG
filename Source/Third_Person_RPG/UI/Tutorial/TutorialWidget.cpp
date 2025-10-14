// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/Tutorial/TutorialWidget.h"
#include "Kismet/GameplayStatics.h"

void UTutorialWidget::CloseWidget()
{
	RemoveFromParent();
}

void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
}

FReply UTutorialWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::Virtual_Accept)
	{
		CloseWidget();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}