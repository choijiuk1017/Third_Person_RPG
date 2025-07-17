// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	bool AddItemByData(UTPRItemData* ItemData, int32 Quantity);

	// 인벤토리 조회
	const TArray<UInventoryItem*>& GetItemsByType(EItemType Type) const;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY()
	UInventoryItem* EquippedWeaponItem;

	UFUNCTION(BlueprintCallable)
	TArray<UInventoryItem*> GetAllItems() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<UInventoryItem*> EquipmentItems;

	UPROPERTY()
	TArray<UInventoryItem*> ConsumableItems;

	UPROPERTY()
	TArray<UInventoryItem*> OtherItems;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxSlotCount = 30;
};
