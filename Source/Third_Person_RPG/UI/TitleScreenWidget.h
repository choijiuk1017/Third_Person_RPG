// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UTitleScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NewGameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LoadGameText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ExitText;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* StartAnim;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoadingScreen;

	UUserWidget* LoadingScreenWidget;


	int32 CurrentIndex = 0;

	void UpdateSelection();

	void ExecuteSelection();
};
