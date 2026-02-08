// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoryCutsceneWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UStoryCutsceneWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void SetSlide(UTexture2D* InTexture, const FText& InText);

protected:
	// UMG 디자이너에서 변수명 동일하게 만들고 "Is Variable" 체크하세요.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> StoryImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StoryText;
	
};
