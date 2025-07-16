// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TPRSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UTPRSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 마지막 저장된 위치
	UPROPERTY(VisibleAnywhere)
	FVector LastSavedLocation;

	// 활성화된 세이브포인트 이름 목록
	UPROPERTY(VisibleAnywhere)
	TArray<FString> ActivatedSavePointNames;
};
