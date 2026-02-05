// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueChoiceWidget.generated.h"

class UTextBlock;
class UBorder;
/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChoiceConfirmed, bool);

UCLASS()
class THIRD_PERSON_RPG_API UDialogueChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDialogueChoiceWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetTexts(const FText& InQuestion, const FText& InYes, const FText& InNo);

	UFUNCTION(BlueprintCallable)
	void SetSelectedIndex(int32 Index);

	UFUNCTION(BlueprintPure)
	int32 GetSelectedIndex() const { return SelectedIndex; }

	FOnChoiceConfirmed OnChoiceConfirmed;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;


protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestionText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* YesText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UBorder* YesHighlight = nullptr;

	UPROPERTY(meta = (BindWidget))
	UBorder* NoHighlight = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Choice")
	int32 SelectedIndex = 0;
	
};
