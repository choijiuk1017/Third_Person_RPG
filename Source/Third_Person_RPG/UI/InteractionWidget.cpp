// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InteractionWidget.h"
#include "Components/TextBlock.h"

void UInteractionWidget::SetHelpText(FString HelpText)
{
	// 텍스트 설정
	TXT_HelpText->SetText(FText::FromString(HelpText));
}