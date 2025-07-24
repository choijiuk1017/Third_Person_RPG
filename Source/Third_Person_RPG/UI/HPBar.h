// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBar.generated.h"

class UProgressBar;


/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UHPBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	void SetHPBarPercent(float Percent);
	
};
