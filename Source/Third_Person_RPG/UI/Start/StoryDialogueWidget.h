// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoryDialogueWidget.generated.h"

class UTextBlock;


/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UStoryDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void SetSlide(const FText& InText);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StoryText;
};
