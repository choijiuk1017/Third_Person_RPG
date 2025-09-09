// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h" 
#include "LevelUpMenu.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API ULevelUpMenu : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentLevelText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentCurrencyText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* LevelUpText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* RemainingCurrencyText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* NeedCurrencyText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentVigorText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentMindText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentEnduranceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentStrengthText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentDexterityText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentIntelligenceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentFaithText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentArcaneText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* VigorText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MindText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* EnduranceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StrengthText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* DexterityText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* IntelligenceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* FaithText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* ArcaneText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_HP;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_FP;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Stamina;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Weight;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Attack;


	UPROPERTY(meta = (BindWidget))
	class UButton* DecisionButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* VigorUpButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* MindUpButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* EnduranceUpButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* StrengthUpButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* DexterityUpButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* IntelligenceUpButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* FaithUpButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* ArcaneUpButton;

};
