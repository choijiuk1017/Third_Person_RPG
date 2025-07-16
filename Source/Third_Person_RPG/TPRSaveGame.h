// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "TPRSaveGame.generated.h"


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
};
