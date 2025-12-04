// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* FadeInAnim;
};
