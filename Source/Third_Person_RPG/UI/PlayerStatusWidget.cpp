// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/PlayerStatusWidget.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SizeBox.h"

void UPlayerStatusWidget::InitWithPlayer(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;

    if (SizeBox_HP && SizeBox_HP->GetWidthOverride() > 0.f)       BaseHPWidth = SizeBox_HP->GetWidthOverride();
    if (SizeBox_FP && SizeBox_FP->GetWidthOverride() > 0.f)       BaseFPWidth = SizeBox_FP->GetWidthOverride();
    if (SizeBox_Stamina && SizeBox_Stamina->GetWidthOverride() > 0.f) BaseStaminaWidth = SizeBox_Stamina->GetWidthOverride();

	if (OwnerPlayer.IsValid())
	{
		OwnerPlayer->OnHPChanged.AddUObject(this, &UPlayerStatusWidget::UpdateHP);
		OwnerPlayer->OnFPChanged.AddUObject(this, &UPlayerStatusWidget::UpdateFP);
		OwnerPlayer->OnStaminaChanged.AddUObject(this, &UPlayerStatusWidget::UpdateStamina);
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
	ApplyWidths(MaxHP, MaxFP, MaxStamina);
}

float UPlayerStatusWidget::CalcWidth(float Base, int32 CurMax, int32 RefMax) const
{
	if (RefMax <= 0) return FMath::Max(MinBarWidth, Base);
	const float Scaled = Base * (static_cast<float>(CurMax) / static_cast<float>(RefMax));
	return FMath::Max(MinBarWidth, Scaled);
}

void UPlayerStatusWidget::ApplyWidths(int32 InMaxHP, int32 InMaxFP, int32 InMaxStamina)
{
	if (SizeBox_HP)      SizeBox_HP->SetWidthOverride(CalcWidth(BaseHPWidth, InMaxHP, RefMaxHP));
	if (SizeBox_FP)      SizeBox_FP->SetWidthOverride(CalcWidth(BaseFPWidth, InMaxFP, RefMaxFP));
	if (SizeBox_Stamina) SizeBox_Stamina->SetWidthOverride(CalcWidth(BaseStaminaWidth, InMaxStamina, RefMaxStamina));

	// 레이아웃 강제 갱신(반영 지연 방지)
	InvalidateLayoutAndVolatility();
}

void UPlayerStatusWidget::ApplyPercents(
	int32 CurHP, int32 MaxHP, int32 CurFP, int32 MaxFP, int32 CurStamina, int32 MaxStamina)
{
	if (HPBar)      HPBar->SetPercent(SafePercent(CurHP, MaxHP));
	if (FPBar)      FPBar->SetPercent(SafePercent(CurFP, MaxFP));
	if (StaminaBar) StaminaBar->SetPercent(SafePercent(CurStamina, MaxStamina));
}
