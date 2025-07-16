// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldTravelMenu.generated.h"

class UVerticalBox;

class UWorldTravelMenuEntry;

UCLASS()
class THIRD_PERSON_RPG_API UWorldTravelMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 외부에서 목록을 채우기 위해 호출할 함수
	void SetTravelPoints(const TArray<FString>& PointNames);

	void NavigateUp();
	void NavigateDown();
	void ConfirmSelection();


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_TravelList;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWorldTravelMenuEntry> TravelEntryClass;

private:
	TArray<UWorldTravelMenuEntry*> EntryWidgets;
	int32 SelectedIndex = 0;

	void UpdateSelectionVisual();

};
