// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/DeathScreenWidget.h"

void UDeathScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeInAnim)
	{
		PlayAnimation(FadeInAnim);
	}
}