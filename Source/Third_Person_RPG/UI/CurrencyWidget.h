// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CurrencyWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UCurrencyWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void SetCurrency(int32 NewValue);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrencyText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency")
	FText Prefix = FText::FromString(TEXT("G "));
};
