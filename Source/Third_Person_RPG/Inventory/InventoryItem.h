// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	//UInventoryItem();

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTPRItemData> ItemData;

	UPROPERTY(VisibleAnywhere)
	int32 ItemQuantity;
};
