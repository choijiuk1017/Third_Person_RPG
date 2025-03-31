// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "WeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UWeaponItemData : public UTPRItemData
{
	GENERATED_BODY()
	
public: 
	//UWeaponItemData();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class ATPRWeapon> WeaponClass;
};
