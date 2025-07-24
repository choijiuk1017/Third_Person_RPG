// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/HPBar.h"
#include "Components/ProgressBar.h"

void UHPBar::SetHPBarPercent(float Percent)
{
	if (HPBar)
	{
		HPBar->SetPercent(Percent);
	}
}