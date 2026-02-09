// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Third_Person_RPG/TPRSaveGame.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Engine/AssetManager.h"

#include "TPRGameInstance.generated.h"

class UTPRSaveGame;

class UTutorialWidget;

USTRUCT(BlueprintType)
struct FInventoryItemSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	UTPRItemData* ItemData;

	UPROPERTY()
	int32 Quantity;

	UPROPERTY()
	bool bEquipped = false;

	UPROPERTY()
	int32 EnhanceLevel = 0;
};

USTRUCT(BlueprintType)
struct FHiddenBossRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPrimaryAssetId> RequiredWeaponAssets;
};


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

	UPROPERTY()
	UUserWidget* LoadingScreenWidget;


	UPROPERTY()
	TSet<FPrimaryAssetId> CollectedItemAssets;

	// 아이템 등록
	UFUNCTION()
	void RegisterCollectedItemAsset(const FPrimaryAssetId& AssetId)
	{
		CollectedItemAssets.Add(AssetId);
	}

	// 아이템 이미 등록되었는지 확인
	UFUNCTION()
	bool HasCollectedItemAsset(const FPrimaryAssetId& AssetId) const
	{
		return CollectedItemAssets.Contains(AssetId);
	}

	void RegisterPlayerStatFromPlayer(const APlayerCharacter* Player);
	void ApplyLoadedPlayerStatTo(APlayerCharacter* Player);

	UPROPERTY()
	TSet<FName> FinishedTutorials;

	void RegisterTutorialWidget(UTutorialWidget* Widget);

	UFUNCTION()
	void OnTutorialFinished(FName TutorialID);

	UPROPERTY(BlueprintReadWrite)
	FName LastRestedSavePointID;

	UPROPERTY()
	TMap<FName, FSavePointInfo> DiscoveredSavePoints;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRespawn = false;

	UPROPERTY(BlueprintReadWrite)
	FVector RespawnLocation;

	UPROPERTY(BlueprintReadWrite)
	FRotator RespawnRotation;

	UFUNCTION()
	void RegisterMetNPC(FName NPCID);

	UFUNCTION()
	bool HasMetNPC(FName NPCID) const;

	UPROPERTY()
	TSet<FName> MetNPCs;


	UFUNCTION()
	void RegisterClearedBoss(FName BossID);

	UFUNCTION()
	bool HasClearedBoss(FName BossID) const;

	UPROPERTY()
	TSet<FName> ClearedBoss;

	UFUNCTION(BlueprintCallable)
	bool DeleteSaveData();

	UPROPERTY(BlueprintReadOnly)
	bool bHiddenBossUnlocked = false;

	UFUNCTION(BlueprintCallable)
	bool CheckAndUnlockHiddenBoss(const TArray<FPrimaryAssetId>& RequiredWeaponAssets);

	UFUNCTION(BlueprintCallable)
	bool IsHiddenBossUnlocked() const { return bHiddenBossUnlocked; }

	UPROPERTY()
	TMap<FPrimaryAssetId, int32> LoadedEnhanceLevels;

protected:


	UPROPERTY()
	FName PendingSavePointID;

	UPROPERTY()
	FString LastActivatedSavePoint;

private:
	FString SaveSlotName = TEXT("PlayerSaveSlot");
	uint32 UserIndex = 0;

	UPROPERTY()
	FPlayerStatSaveData CachedPlayerStat;

	bool bHasLoadedStat = false;

};
