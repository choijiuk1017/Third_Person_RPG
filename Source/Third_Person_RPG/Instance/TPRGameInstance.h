// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TPRGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UTPRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 발견한 세이브포인트 등록
	void RegisterSavePoint(const FString& Name, const FVector& Location);

	// 세이브포인트 목록 가져오기
	const TMap<FString, FVector>& GetSavePointMap() const;

	// 모든 등록된 세이브포인트 삭제
	void ClearSavePoints();

protected:
	UPROPERTY()
	TMap<FString, FVector> DiscoveredSavePoints;
};
