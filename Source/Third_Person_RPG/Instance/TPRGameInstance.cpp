// Fill out your copyright notice in the Description page of Project Settings.
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/TPRSaveGame.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"


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

		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn()))
			{
				FPlayerStatSaveData Snapshot;
				Player->FillSaveData(Snapshot);
				SaveGameInstance->PlayerStat = Snapshot;

				// 메모리 캐시도 최신으로 유지
				CachedPlayerStat = Snapshot;
				bHasLoadedStat = true;
			}
		}

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, UserIndex);
	}
}

bool UTPRGameInstance::LoadGameData()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		bHasLoadedStat = false;
		return false;
	}

	UTPRSaveGame* LoadedGame = Cast<UTPRSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	if (!LoadedGame)
	{
		bHasLoadedStat = false;
		return false;
	}

	DiscoveredSavePoints.Empty();
	for (const FSavePointInfo& Info : LoadedGame->AllDiscoveredSavePoints)
	{
		DiscoveredSavePoints.Add(Info.SavePointID, Info);
	}

	CachedPlayerStat = LoadedGame->PlayerStat;
	bHasLoadedStat = true;

	return true;
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