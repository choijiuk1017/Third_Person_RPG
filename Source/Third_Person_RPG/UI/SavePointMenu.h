// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Third_Person_RPG/UI/SavePointMenuEntry.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"

#include "SavePointMenu.generated.h"

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

	virtual void NativeConstruct() override;

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
private:
	TArray<USavePointMenuEntry*> EntryWidgets;

	int32 SelectedIndex = 0;
	void UpdateSelectionVisual();
	
};
