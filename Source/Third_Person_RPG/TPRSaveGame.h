// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "TPRSaveGame.generated.h"


USTRUCT(BlueprintType)
struct FPlayerStatSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FCharacterAttributes BaseAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	FDerivedStats DerivedStats;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	//FCombatStats CombatStats; // HP/FP/Stamina의 Current 값 포함된 걸로 가정

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 Currency = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")

	int32 SpentCurrencyOnStats = 0;
};
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UTPRSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FVector LastSavedLocation;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> ActivatedSavePointNames;

	UPROPERTY(VisibleAnywhere)
	TArray<FSavePointInfo> AllDiscoveredSavePoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Save")
	FPlayerStatSaveData PlayerStat;

	UPROPERTY()
	FName LastSavedMapName;


	UPROPERTY()
	TSet<FName> MetNPCs;

	UPROPERTY()
	TSet<FName> ClearedBoss;

	UPROPERTY()
	bool bHiddenBossUnlocked = false;

	UPROPERTY()
	TSet<FPrimaryAssetId> CollectedItemAssets;

	UPROPERTY()
	TMap<FPrimaryAssetId, int32> ItemEnhanceLevels;

	UPROPERTY()
	FPrimaryAssetId EquippedWeaponAssetId;
};
