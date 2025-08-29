// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/CurrencyWidget.h"
#include "Components/TextBlock.h"

void UCurrencyWidget::SetCurrency(int32 NewValue)
{
	if (!CurrencyText) return;

	const FText ValueText = FText::AsNumber(NewValue);
	if (!Prefix.IsEmpty())
	{
		CurrencyText->SetText(FText::Format(FText::FromString(TEXT("{0}{1}")), Prefix, ValueText));
	}
	else
	{
		CurrencyText->SetText(ValueText);
	}
}
