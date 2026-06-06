// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUIComponent.generated.h"

class APlayerCharacter;
class UWeaponItemData;
class UTexture2D;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerUIComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerCharacter* OwnerPlayer = nullptr;

public:
	void InitializeUIComponent(APlayerCharacter* InOwner);

	void ShowInteractionUI(const FText& InText);
	void HideInteractionUI();

	void SetStatusHUDVisible(bool bVisible);

	void ToggleInventory();
	void PopUpInventory();
	void CloseInventory();

	void ShowWeaponInfo(UWeaponItemData* WeaponData);

	void RefreshCurrentEquipped_Weapon(UTexture2D* WeaponIconTexture);
	void RefreshCurrentEquipped_Potion(UTexture2D* PotionIconTexture, int32 NewCount);
		
};
