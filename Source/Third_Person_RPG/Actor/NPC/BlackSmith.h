// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Actor/NPC.h"
#include "BlackSmith.generated.h"

/**
 * 
 */

class UUserWidget;
class APlayerCharacter;
class UInventoryItem;
class UWeaponUpgradeWidget;

UCLASS()
class THIRD_PERSON_RPG_API ABlackSmith : public ANPC
{
	GENERATED_BODY()

protected:

	virtual void OnTalkFinished(APlayerCharacter* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "BlackSmith|UI")
	TSubclassOf<UUserWidget> UpgradeWidgetClass;

	UPROPERTY()
	TObjectPtr<UWeaponUpgradeWidget> UpgradeWidgetInstance;

	UPROPERTY()
	bool bUpgradeUIOpened = false;

	UFUNCTION()
	void OnUpgradeWidgetClosed();
};
