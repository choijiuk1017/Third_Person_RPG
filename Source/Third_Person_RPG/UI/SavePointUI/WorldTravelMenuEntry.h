// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldTravelMenuEntry.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWorldTravelMenuEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPointName(const FString& NewName);
	const FString& GetPointName() const { return PointName; }

	// 선택된 상태 시 시각적으로 표시 (ex. 색상 변경)
	void SetSelected(bool bSelected);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_PointName;

private:
	FString PointName;
};
