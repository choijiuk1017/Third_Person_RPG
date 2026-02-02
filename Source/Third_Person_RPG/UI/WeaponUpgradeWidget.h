// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponUpgradeWidget.generated.h"

class APlayerCharacter;
class UListView;
class UTextBlock;
class UButton;
class UInventoryItem;
class UWeaponListItemObject;
class UInventoryComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpgradeWidgetClosed);
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWeaponUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "BlackSmith|UI")
	void InitUpgradeUI(APlayerCharacter* InPlayer, const TArray<UInventoryItem*>& InWeapons);

	UPROPERTY(BlueprintAssignable, Category = "BlackSmith|UI")
	FOnUpgradeWidgetClosed OnClosed;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UListView> WeaponListView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> OwnedCurrencyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NeededCurrencyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UpgradeButton;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> Player;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> CachedInvComp;

	UPROPERTY()
	TArray<TObjectPtr<UWeaponListItemObject>> ListItems;

	UPROPERTY()
	TObjectPtr<UWeaponListItemObject> SelectedItemObject;

	UFUNCTION()
	void OnWeaponSelectionChanged(UObject* SelectedItem);

	UFUNCTION()
	void OnUpgradeClicked();

	UFUNCTION()
	void RefreshFromInventory(); 

	void CloseWidget();

	void RebuildList(const TArray<UInventoryItem*>& Weapons);
	void RefreshRightPanel();

	int32 CalcNeededCurrency(const UInventoryItem* Item) const;
	bool CanUpgrade(const UInventoryItem* Item, int32& OutNeededCurrency) const;
	void DoUpgrade(UInventoryItem* Item, int32 NeededCurrency);
};
