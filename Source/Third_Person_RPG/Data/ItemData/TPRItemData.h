// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TPRItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Weapon,
	IT_Potion,
	IT_Consumable,
	IT_Other,
	IT_None

};

UCLASS()
class THIRD_PERSON_RPG_API UTPRItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TPRItemData", GetFName());
	}

public:
	UPROPERTY(EditAnywhere, Category = Base)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = Base)

	TObjectPtr<UTexture2D> ItemTexture;

	UPROPERTY(EditAnywhere, Category = Base)
	FString ItemName;




};
