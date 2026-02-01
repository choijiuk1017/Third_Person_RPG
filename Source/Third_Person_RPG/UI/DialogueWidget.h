// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetDialogueText(const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueText;
};
