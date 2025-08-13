// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/PlayerStatusWidget.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/SizeBox.h"

void UPlayerStatusWidget::InitWithPlayer(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
	// 초기 퍼센트 세팅
	if (OwnerPlayer.IsValid())
	{
		UpdateHP(OwnerPlayer->CombatStats.CurrentHP, OwnerPlayer->DerivedStats.MaxHP);
		UpdateFP(OwnerPlayer->CombatStats.CurrentFP, OwnerPlayer->DerivedStats.MaxFP);
		UpdateStamina(OwnerPlayer->CombatStats.CurrentStamina, OwnerPlayer->DerivedStats.MaxStamina);
	}
}

void UPlayerStatusWidget::UpdateHP(int32 Current, int32 Max)
{
	if (HPBar) HPBar->SetPercent(SafePercent(Current, Max));
}

void UPlayerStatusWidget::UpdateFP(int32 Current, int32 Max)
{
	if (FPBar) FPBar->SetPercent(SafePercent(Current, Max));
}

void UPlayerStatusWidget::UpdateStamina(int32 Current, int32 Max)
{
	if (StaminaBar) StaminaBar->SetPercent(SafePercent(Current, Max));
}

void UPlayerStatusWidget::UpdateBarLengths(int32 MaxHP, int32 MaxFP, int32 MaxStamina)
{
    if (HPBar)
    {
        float NewWidth = 200.f + (MaxHP - 300) * 0.5f;
        if (USizeBox* HPSizeBox = Cast<USizeBox>(HPBar->GetParent()))
        {
            HPSizeBox->SetWidthOverride(NewWidth);
        }
    }

    if (FPBar)
    {
        float NewWidth = 150.f + (MaxFP - 50) * 1.0f;
        if (USizeBox* FPSizeBox = Cast<USizeBox>(FPBar->GetParent()))
        {
            FPSizeBox->SetWidthOverride(NewWidth);
        }
    }

    if (StaminaBar)
    {
        float NewWidth = 180.f + (MaxStamina - 80) * 0.7f;
        if (USizeBox* StaSizeBox = Cast<USizeBox>(StaminaBar->GetParent()))
        {
            StaSizeBox->SetWidthOverride(NewWidth);
        }
    }
}
