// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "PotionItemData.generated.h"

UENUM(BlueprintType)
enum class EPotionType : uint8
{
	PT_Hp,
	PT_Mp,

};
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UPotionItemData : public UTPRItemData
{
	GENERATED_BODY()
	
public:
	//UPotionItemData();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Potion)
	EPotionType PotionType;

	UPROPERTY(EditAnywhere, Category = Potion)
	float Percent;
};
