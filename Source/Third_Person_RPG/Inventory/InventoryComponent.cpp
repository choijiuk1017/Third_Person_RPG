// Fill out your copyright notice in the Description page of Project Settings
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Engine/AssetManager.h"



UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquipmentItems.Init(nullptr, MaxSlotCount);
	ConsumableItems.Init(nullptr, MaxSlotCount);
	OtherItems.Init(nullptr, MaxSlotCount);
}



void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

bool UInventoryComponent::AddItemByData(UTPRItemData* Data, int32 Quantity)
{

	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("AddItemByData: Data가 nullptr입니다."));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("AddItemByData : name: %s, type: %d, count: %d"),
		*Data->ItemName, static_cast<uint8>(Data->ItemType), Quantity);

	UInventoryItem* NewItem = NewObject<UInventoryItem>(this);
	NewItem->ItemData = Data;
	NewItem->Quantity = Quantity;

	TArray<UInventoryItem*>* TargetArray = nullptr;

	switch (Data->ItemType)
	{
	case EItemType::IT_Weapon:     TargetArray = &EquipmentItems; break;
	case EItemType::IT_Potion:     TargetArray = &ConsumableItems; break;
	default:                       TargetArray = &OtherItems; break;
	}



	for (int32 i = 0; i < TargetArray->Num(); ++i)
	{
		if (!IsValid((*TargetArray)[i]))
		{
			(*TargetArray)[i] = NewItem;
			OnInventoryChanged.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("delegate"));
			return true;
		}
	}

	return false;
}

const TArray<UInventoryItem*>& UInventoryComponent::GetItemsByType(EItemType Type) const
{
	switch (Type)
	{
	case EItemType::IT_Weapon:
		UE_LOG(LogTemp, Warning, TEXT("GetItemsByType: IT_Weapon %d개"), EquipmentItems.Num());
		return EquipmentItems;
	case EItemType::IT_Potion:
		UE_LOG(LogTemp, Warning, TEXT("GetItemsByType: IT_Potion %d개"), ConsumableItems.Num());
		return ConsumableItems;
	default:
		UE_LOG(LogTemp, Warning, TEXT("GetItemsByType: Other %d개"), OtherItems.Num());
		return OtherItems;
	}
}
