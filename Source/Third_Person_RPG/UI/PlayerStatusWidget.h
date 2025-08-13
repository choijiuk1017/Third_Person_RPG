// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
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

	UPROPERTY(meta = (BindWidgetOptional)) UImage* FrameImage = nullptr;

	UPROPERTY() TWeakObjectPtr<APlayerCharacter> OwnerPlayer;


	static float SafePercent(int32 Current, int32 Max)
	{
		if (Max <= 0) return 0.f;
		return FMath::Clamp(static_cast<float>(Current) / static_cast<float>(Max), 0.f, 1.f);
	}
};
