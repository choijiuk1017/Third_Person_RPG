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
	FText WeaponName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText WeaponType = FText::GetEmpty(); // 참격, 타격, 관통 등

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Weight = 0.f;

	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Physical = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Magic = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Fire = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Lightning = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	int32 Holy = 0;

	// 스탯 보정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString StrengthScaling = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString DexterityScaling = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString IntelligenceScaling = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString FaithScaling = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Scaling")
	FString ArcaneScaling = TEXT("");

	// 요구 스탯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredStrength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredDexterity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredIntelligence = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredFaith = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Requirements")
	int32 RequiredArcane = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Skill")
	int32 FPCost = 0;
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
