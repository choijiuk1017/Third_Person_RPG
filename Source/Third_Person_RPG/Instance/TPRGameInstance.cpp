// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/TPRSaveGame.h"
#include "Third_Person_RPG/UI/Tutorial/TutorialWidget.h"

void UTPRGameInstance::Init()
{
	Super::Init();
	LoadGameData();
}

void UTPRGameInstance::RegisterSavePoint(const FSavePointInfo& SavePointInfo)
{
	if (!DiscoveredSavePoints.Contains(SavePointInfo.SavePointID))
	{
		DiscoveredSavePoints.Add(SavePointInfo.SavePointID, SavePointInfo);
	}

	UE_LOG(LogTemp, Warning, TEXT("RegisterSavePoint: %s, %s, %s"),
		*SavePointInfo.SavePointID.ToString(),
		*SavePointInfo.DisplayName.ToString(),
		*SavePointInfo.MapName.ToString());
}

const TMap<FName, FSavePointInfo>& UTPRGameInstance::GetSavePointMap() const
{
	return DiscoveredSavePoints;
}

TArray<FString> UTPRGameInstance::GetActivatedSavePointNames() const
{
	TArray<FString> Names;
	for (const auto& Pair : DiscoveredSavePoints)
	{
		Names.Add(Pair.Key.ToString());
	}
	return Names;
}

void UTPRGameInstance::ClearSavePoints()
{
	DiscoveredSavePoints.Empty();
}

void UTPRGameInstance::SaveGameData()
{
	UTPRSaveGame* SaveGameInstance = Cast<UTPRSaveGame>(UGameplayStatics::CreateSaveGameObject(UTPRSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->LastSavedMapName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld(), true));

		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				SaveGameInstance->LastSavedLocation = Pawn->GetActorLocation();
			}
		}

		for (const auto& Pair : DiscoveredSavePoints)
		{
			SaveGameInstance->AllDiscoveredSavePoints.Add(Pair.Value);
			SaveGameInstance->ActivatedSavePointNames.Add(Pair.Key.ToString());
		}

		SaveGameInstance->PlayerStat = CachedPlayerStat;

		SaveGameInstance->MetNPCs = MetNPCs;

		SaveGameInstance->ClearedBoss = ClearedBoss;

		SaveGameInstance->CollectedItemAssets = CollectedItemAssets;

		SaveGameInstance->bHiddenBossUnlocked = bHiddenBossUnlocked;

		SaveGameInstance->ItemEnhanceLevels.Empty();
		for (const FInventoryItemSaveData& It : CachedInventoryItems)
		{
			if (!It.ItemData) continue;

			const FPrimaryAssetId Id = It.ItemData->GetPrimaryAssetId();
			SaveGameInstance->ItemEnhanceLevels.Add(Id, It.EnhanceLevel); // ¡Ú ÇÙ½É
		}

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
	}
}

bool UTPRGameInstance::LoadGameData()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		UTPRSaveGame* LoadedGame = Cast<UTPRSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
		if (LoadedGame)
		{
			DiscoveredSavePoints.Empty();

			for (const FSavePointInfo& Info : LoadedGame->AllDiscoveredSavePoints)
			{
				DiscoveredSavePoints.Add(Info.SavePointID, Info);
			}

			CachedPlayerStat = LoadedGame->PlayerStat;

			MetNPCs = LoadedGame->MetNPCs;

			bHasLoadedStat = true;

			CollectedItemAssets = LoadedGame->CollectedItemAssets;
			bHiddenBossUnlocked = LoadedGame->bHiddenBossUnlocked;

			ClearedBoss = LoadedGame->ClearedBoss;

			LoadedEnhanceLevels = LoadedGame->ItemEnhanceLevels;

			return true;
		}
	}
	return false;
}

void UTPRGameInstance::CacheInventory(const TArray<UInventoryItem*>& Items)
{
	CachedInventoryItems.Empty();
	for (UInventoryItem* Item : Items)
	{
		if (Item && Item->ItemData)
		{
			FInventoryItemSaveData SaveData;
			SaveData.ItemData = Item->ItemData;
			SaveData.Quantity = Item->Quantity;
			SaveData.bEquipped = Item->bEquipped;

			SaveData.EnhanceLevel = Item->EnhanceLevel;

			CachedInventoryItems.Add(SaveData);
		}
	}
}

const TArray<FInventoryItemSaveData>& UTPRGameInstance::GetCachedInventory() const
{
	return CachedInventoryItems;
}

void UTPRGameInstance::RegisterPlayerStatFromPlayer(const APlayerCharacter* Player)
{
	if (!Player) return;

	Player->FillSaveData(CachedPlayerStat);
	bHasLoadedStat = true;
}

void UTPRGameInstance::ApplyLoadedPlayerStatTo(APlayerCharacter* Player)
{
	if (!Player) return;

	if (!bHasLoadedStat) return;

	Player->ApplySaveData(CachedPlayerStat);
}

void UTPRGameInstance::RegisterTutorialWidget(UTutorialWidget* Widget)
{
	if (!Widget) return;
	Widget->OnTutorialFinished.AddDynamic(this, &UTPRGameInstance::OnTutorialFinished);
}

void UTPRGameInstance::OnTutorialFinished(FName TutorialID)
{
	if (!FinishedTutorials.Contains(TutorialID))
	{
		FinishedTutorials.Add(TutorialID);
	}
}

void UTPRGameInstance::RegisterMetNPC(FName NPCID)
{
	if (NPCID.IsNone()) return;
	MetNPCs.Add(NPCID);
}

bool UTPRGameInstance::HasMetNPC(FName NPCID) const
{
	if (NPCID.IsNone()) return false;
	return MetNPCs.Contains(NPCID);
}

void UTPRGameInstance::RegisterClearedBoss(FName BossID)
{
	if (BossID.IsNone()) return;
	ClearedBoss.Add(BossID);
}

bool UTPRGameInstance::HasClearedBoss(FName BossID) const
{
	if (BossID.IsNone()) return false;
	return ClearedBoss.Contains(BossID);
}

bool UTPRGameInstance::DeleteSaveData()
{
	const bool bDeleted = UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);

	DiscoveredSavePoints.Empty();
	CachedInventoryItems.Empty();
	MetNPCs.Empty();
	ClearedBoss.Empty();
	bHasLoadedStat = false;
	PendingSavePointID = NAME_None;
	LastRestedSavePointID = NAME_None;

	return bDeleted;
}


bool UTPRGameInstance::CheckAndUnlockHiddenBoss(const TArray<FPrimaryAssetId>& RequiredWeaponAssets)
{
	if (bHiddenBossUnlocked) return true;

	for (const FPrimaryAssetId& Req : RequiredWeaponAssets)
	{
		if (!CollectedItemAssets.Contains(Req))
		{
			return false;
		}
	}

	bHiddenBossUnlocked = true;

	return true;
}

