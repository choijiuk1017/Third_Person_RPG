// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UTextBlock> TXT_HelpText;
	
	void SetHelpText(FString HelpText);
};
