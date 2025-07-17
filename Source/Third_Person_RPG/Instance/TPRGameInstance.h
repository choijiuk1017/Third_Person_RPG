// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Third_Person_RPG/TPRSaveGame.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"

#include "TPRGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	UTPRItemData* ItemData;

	UPROPERTY()
	int32 Quantity;
};

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UTPRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void RegisterSavePoint(const FSavePointInfo& SavePointInfo);
	const TMap<FName, FSavePointInfo>& GetSavePointMap() const;
	void ClearSavePoints();

	// 세이브 및 로드 기능 추가
	void SaveGameData();
	bool LoadGameData();

	TArray<FString> GetActivatedSavePointNames() const;

	void SetPendingSavePoint(FName InID) { PendingSavePointID = InID; }
	FName GetPendingSavePoint() const { return PendingSavePointID; }
	void ClearPendingSavePoint() { PendingSavePointID = NAME_None; }

	UPROPERTY()
	TArray<FInventoryItemSaveData> CachedInventoryItems;

	void CacheInventory(const TArray<UInventoryItem*>& Items);
	const TArray<FInventoryItemSaveData>& GetCachedInventory() const;

protected:
	UPROPERTY()
	TMap<FName, FSavePointInfo> DiscoveredSavePoints;

	UPROPERTY()
	FName PendingSavePointID;

	UPROPERTY()
	FString LastActivatedSavePoint;

private:
	FString SaveSlotName = TEXT("PlayerSaveSlot");
	uint32 UserIndex = 0;

};
