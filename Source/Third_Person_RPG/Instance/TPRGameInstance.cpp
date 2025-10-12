// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/TPRSaveGame.h"

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
		OnSavePointRegistered.Broadcast(SavePointInfo);
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

		SaveGameInstance->PlayerStat = CachedPlayerStat;


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
			bHasLoadedStat = true;
			return true;

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

	// 플레이어가 가진 현재 스탯을 통으로 받아 캐시에 보관
	Player->FillSaveData(CachedPlayerStat);
	bHasLoadedStat = true;
}

void UTPRGameInstance::ApplyLoadedPlayerStatTo(APlayerCharacter* Player)
{
	if (!Player) return;

	// 세이브 파일이 없거나 캐시가 없으면 적용하지 않음
	if (!bHasLoadedStat) return;

	Player->ApplySaveData(CachedPlayerStat);
}