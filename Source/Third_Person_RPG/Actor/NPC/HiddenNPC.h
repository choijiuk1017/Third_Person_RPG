// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Actor/NPC.h"
#include "HiddenNPC.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API AHiddenNPC : public ANPC
{
	GENERATED_BODY()
public:
	AHiddenNPC();

protected:
	// 이 NPC가 요구하는 필수 무기 목록 (에디터에서 지정)
	UPROPERTY(EditAnywhere, Category = "HiddenBoss")
	TArray<FPrimaryAssetId> RequiredWeaponAssets;

	UPROPERTY(EditAnywhere, Category = "Dialogue|Choice")
	FText NeedMoreWeaponText;

	UPROPERTY(EditAnywhere, Category = "Dialogue|Choice")
	FText HiddenUnlockText;

	UPROPERTY(EditAnywhere, Category = "Dialogue|Choice")
	FText YesConfirmText;


	virtual void OnYesSelected(APlayerCharacter* Player) override;
	virtual void OnNoSelected(APlayerCharacter* Player) override;
	
};
