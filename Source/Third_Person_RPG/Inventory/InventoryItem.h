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

	UTexture2D* GetItemTexture() const
	{
		return ItemData ? ItemData->ItemTexture : nullptr;
	}
};
