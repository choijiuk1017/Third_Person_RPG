// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/Start/StoryDialogueWidget.h"

#include "Components/TextBlock.h"
void UStoryDialogueWidget::SetSlide(const FText& InText)
{
	SetVisibility(ESlateVisibility::Visible);


	if (StoryText)
	{
		StoryText->SetText(InText);
	}
}