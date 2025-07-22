// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "WeaponItemData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStatData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText WeaponType; // 참격, 타격, 관통 등

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Weight;

	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Physical;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Magic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Lightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Holy;

	// 스탯 보정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString StrengthScaling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString DexterityScaling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString IntelligenceScaling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString FaithScaling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString ArcaneScaling;

	// 요구 스탯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredDexterity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredIntelligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredFaith;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredArcane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Skill")
	int32 FPCost;
};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponStats)
	FWeaponStatData WeaponStats;
};
