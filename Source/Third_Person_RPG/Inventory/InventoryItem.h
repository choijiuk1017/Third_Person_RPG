// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "InventoryItem.generated.h"

UCLASS(Blueprintable)
class THIRD_PERSON_RPG_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UTPRItemData* ItemData;

	UPROPERTY()
	int32 Quantity;

	UPROPERTY()
	bool bEquipped = false;

	UPROPERTY(BlueprintReadWrite, Category = "Enhance")
	int32 EnhanceLevel = 0;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UTPRItemData* GetItemData() const { return ItemData; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	EItemType GetItemType() const { return ItemData ? ItemData->ItemType : EItemType::IT_None; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UTexture2D* GetItemTexture() const
	{
		return ItemData ? ItemData->ItemTexture : nullptr;
	}

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FString GetItemName() const
	{
		return ItemData ? ItemData->ItemName : FString();
	}
};
