// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WeaponUpgradeEntryWidget.generated.h"

class UTextBlock;
class UImage;
class UBorder;
class UWeaponListItemObject;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWeaponUpgradeEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	UFUNCTION()
	UObject* GetMyListItemObject() const;

	UFUNCTION()
	void SetSelectedVisual(bool bSelected);

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnhanceLevelText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NeededCurrencyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AffordableText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SelectionBorder;

	

	
private:
	UPROPERTY()
	TObjectPtr<UWeaponListItemObject> CachedItemObject;

	void Refresh();

};
