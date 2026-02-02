// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WeaponUpgradeEntryWidget.generated.h"

class UTextBlock;
class UImage;
class UWeaponListItemObject;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWeaponUpgradeEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

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

private:
	UPROPERTY()
	TObjectPtr<UWeaponListItemObject> CachedItemObject;

	void Refresh();
};
