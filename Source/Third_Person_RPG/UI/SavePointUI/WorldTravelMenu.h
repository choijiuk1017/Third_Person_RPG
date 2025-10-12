// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "WorldTravelMenu.generated.h"

class UVerticalBox;
class UImage;
class UWorldTravelMenuEntry;
class UButton;

UCLASS()
class THIRD_PERSON_RPG_API UWorldTravelMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UWorldTravelMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// 외부에서 목록을 채우기 위해 호출할 함수
	void SetTravelPoints(const TArray<FSavePointInfo>& SavePoints);

	void NavigateUp();
	void NavigateDown();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void ConfirmSelection();


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_TravelList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PreviewImage;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWorldTravelMenuEntry> TravelEntryClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoadingScreen;

	UUserWidget* LoadingScreenWidget;

	UPROPERTY(meta = (BindWidget)) UButton* ExitButton;

private:
	TArray<UWorldTravelMenuEntry*> EntryWidgets;
	int32 SelectedIndex = 0;

	void UpdateSelectionVisual();
	void UpdatePreviewImage(const FString& PointName);

	UFUNCTION() void OnExitClicked();
	void CloseMenuToGameOnly();

};
