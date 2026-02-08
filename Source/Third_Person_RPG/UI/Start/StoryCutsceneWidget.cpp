// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/Start/StoryCutsceneWidget.h"


#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UStoryCutsceneWidget::SetSlide(UTexture2D* InTexture, const FText& InText)
{
	SetVisibility(ESlateVisibility::Visible);

	if (StoryText)
	{
		StoryText->SetText(InText);
	}

	if (StoryImage)
	{
		StoryImage->SetBrushFromTexture(InTexture, true);
	}
}