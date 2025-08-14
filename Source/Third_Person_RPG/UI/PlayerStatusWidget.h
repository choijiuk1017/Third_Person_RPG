// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/SizeBox.h" 
#include "PlayerStatusWidget.generated.h"

class APlayerCharacter;

UCLASS()
class THIRD_PERSON_RPG_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitWithPlayer(APlayerCharacter* InOwner);


	void UpdateHP(int32 Current, int32 Max);
	void UpdateFP(int32 Current, int32 Max);
	void UpdateStamina(int32 Current, int32 Max);

	UFUNCTION(BlueprintCallable)
	void UpdateBarLengths(int32 MaxHP, int32 MaxFP, int32 MaxStamina);

protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* HPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* FPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* StaminaBar = nullptr;

	UPROPERTY(meta = (BindWidget)) USizeBox* SizeBox_HP = nullptr;
	UPROPERTY(meta = (BindWidget)) USizeBox* SizeBox_FP = nullptr;
	UPROPERTY(meta = (BindWidget)) USizeBox* SizeBox_Stamina = nullptr;

	UPROPERTY(meta = (BindWidgetOptional)) UImage* FrameImage = nullptr;

	UPROPERTY() TWeakObjectPtr<APlayerCharacter> OwnerPlayer;


	static float SafePercent(int32 Current, int32 Max)
	{
		if (Max <= 0) return 0.f;
		return FMath::Clamp(static_cast<float>(Current) / static_cast<float>(Max), 0.f, 1.f);
	}

	UPROPERTY(EditAnywhere, Category = "Status|Bars") float BaseHPWidth = 200.f;
	UPROPERTY(EditAnywhere, Category = "Status|Bars") float BaseFPWidth = 160.f;
	UPROPERTY(EditAnywhere, Category = "Status|Bars") float BaseStaminaWidth = 160.f;

	UPROPERTY(EditAnywhere, Category = "Status|Bars") int32 RefMaxHP = 100;
	UPROPERTY(EditAnywhere, Category = "Status|Bars") int32 RefMaxFP = 50;
	UPROPERTY(EditAnywhere, Category = "Status|Bars") int32 RefMaxStamina = 50;

	UPROPERTY(EditAnywhere, Category = "Status|Bars") float MinBarWidth = 60.f;

	float CalcWidth(float Base, int32 CurMax, int32 RefMax) const;
	void ApplyWidths(int32 MaxHP, int32 MaxFP, int32 MaxStamina);
	void ApplyPercents(int32 CurHP, int32 MaxHP, int32 CurFP, int32 MaxFP, int32 CurStamina, int32 MaxStamina);
};
