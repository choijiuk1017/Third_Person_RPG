// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Third_Person_RPG/TPRSaveGame.h"
#include "Third_Person_RPG/Actor/SavePoint.h"

#include "TPRGameInstance.generated.h"
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
	const TMap<FString, FSavePointInfo>& GetSavePointMap() const;
	void ClearSavePoints();

	// 세이브 및 로드 기능 추가
	void SaveGameData();
	bool LoadGameData();

	TArray<FString> GetActivatedSavePointNames() const;

protected:
	UPROPERTY()
	TMap<FString, FSavePointInfo> DiscoveredSavePoints;

	UPROPERTY()
	FString LastActivatedSavePoint;

private:
	FString SaveSlotName = TEXT("PlayerSaveSlot");
	uint32 UserIndex = 0;

};
