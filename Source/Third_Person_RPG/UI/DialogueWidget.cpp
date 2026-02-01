// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/DialogueWidget.h"
#include "Components/TextBlock.h"

void UDialogueWidget::SetDialogueText(const FText& InText)
{
	if (DialogueText)
	{
		DialogueText->SetText(InText);
	}

}