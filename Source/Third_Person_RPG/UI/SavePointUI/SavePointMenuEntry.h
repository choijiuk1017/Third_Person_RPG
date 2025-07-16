// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SavePointMenuEntry.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API USavePointMenuEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	// 텍스트 설정 함수
	UFUNCTION(BlueprintCallable)
	void SetMenuText(const FText& InText);

	// 선택 상태 설정 함수
	UFUNCTION(BlueprintCallable)
	void SetSelected(bool bSelected);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_MenuName;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border_Highlight;
	
};
