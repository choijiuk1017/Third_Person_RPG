// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponListItemObject.generated.h"

class UInventoryItem;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWeaponListItemObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<UInventoryItem> Item = nullptr;

	UPROPERTY()
	int32 NeededCurrency = 0;

	UPROPERTY()
	bool bAffordable = false;
};
