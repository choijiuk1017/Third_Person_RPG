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
	if (!DiscoveredSavePoints.Contains(SavePointInfo.SavePointID.ToString()))
	{
		DiscoveredSavePoints.Add(SavePointInfo.SavePointID.ToString(), SavePointInfo);
	}
}

const TMap<FString, FSavePointInfo>& UTPRGameInstance::GetSavePointMap() const
{
	return DiscoveredSavePoints;
}

TArray<FString> UTPRGameInstance::GetActivatedSavePointNames() const
{
	TArray<FString> Keys;
	DiscoveredSavePoints.GetKeys(Keys);
	return Keys;
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
			SaveGameInstance->ActivatedSavePointNames.Add(Pair.Key);
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
				DiscoveredSavePoints.Add(Info.SavePointID.ToString(), Info);
			}

			return true;
		}
	}
	return false;
}