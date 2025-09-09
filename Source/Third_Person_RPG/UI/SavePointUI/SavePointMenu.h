// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Third_Person_RPG/UI/SavePointUI/SavePointMenuEntry.h"

#include "SavePointMenu.generated.h"

class APlayerCharacter;
class UVerticalBox;


/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API USavePointMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	APlayerCharacter* OwningActor;

	USavePointMenu(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// 입력 처리용 함수
	void MoveSelectionUp();
	void MoveSelectionDown();
	void ConfirmSelection();


protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VerticalBox_MenuList;

	UPROPERTY(EditAnywhere, Category = "Menu")
	TArray<FText> MenuOptions;

	UPROPERTY(EditAnywhere, Category = "Menu")
	TSubclassOf<USavePointMenuEntry> EntryClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UWorldTravelMenu> WorldTravelMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULevelUpMenu> LevelUpMenuClass;

private:
	TArray<USavePointMenuEntry*> EntryWidgets;

	int32 SelectedIndex = 0;
	void UpdateSelectionVisual();
	
};
