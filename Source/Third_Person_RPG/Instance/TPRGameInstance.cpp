// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Instance/TPRGameInstance.h"

void UTPRGameInstance::RegisterSavePoint(const FString& Name, const FVector& Location)
{
	if (!DiscoveredSavePoints.Contains(Name))
	{
		DiscoveredSavePoints.Add(Name, Location);
	}
}

const TMap<FString, FVector>& UTPRGameInstance::GetSavePointMap() const
{
	return DiscoveredSavePoints;
}

void UTPRGameInstance::ClearSavePoints()
{
	DiscoveredSavePoints.Empty();
}