// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Blueprint/UserWidget.h"
#include "Third_Person_RPG/Actor/SavePoint.h"

#include "WorldTravelMenuEntry.generated.h"

DECLARE_DELEGATE_OneParam(FOnEntrySelected, const FString&);
class UBorder;
class UTextBlock;



/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWorldTravelMenuEntry : public UUserWidget
{
	GENERATED_BODY()


public:
	void SetSavePointInfo(const FSavePointInfo& Info);
	FSavePointInfo GetSavePointInfo() const;

	void SetSelected(bool bSelected);

	FOnEntrySelected OnSelected;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PointName;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border_Highlight;

private:
	FSavePointInfo SavePointInfo;

};
