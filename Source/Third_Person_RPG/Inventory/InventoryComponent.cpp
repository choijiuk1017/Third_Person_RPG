// Fill out your copyright notice in the Description page of Project Settings
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
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

UInventoryItem* UInventoryComponent::AddItemByData(UTPRItemData* Data, int32 Quantity, bool bShouldEquip)
{
	if (!Data) return nullptr;

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

			if (bShouldEquip && GetOwner()->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
			{
				IInventoryInterface::Execute_EquipWeapon(GetOwner(), NewItem);
				NewItem->bEquipped = true;
			}

			return NewItem;
		}
	}

	return nullptr;
}

const TArray<UInventoryItem*>& UInventoryComponent::GetItemsByType(EItemType Type) const
{
	switch (Type)
	{
	case EItemType::IT_Weapon:
		UE_LOG(LogTemp, Warning, TEXT("GetItemsByType: IT_Weapon %d°³"), EquipmentItems.Num());
		return EquipmentItems;
	case EItemType::IT_Potion:
		UE_LOG(LogTemp, Warning, TEXT("GetItemsByType: IT_Potion %d°³"), ConsumableItems.Num());
		return ConsumableItems;
	default:
		UE_LOG(LogTemp, Warning, TEXT("GetItemsByType: Other %d°³"), OtherItems.Num());
		return OtherItems;
	}
}


TArray<UInventoryItem*> UInventoryComponent::GetAllItems() const
{
	TArray<UInventoryItem*> AllItems;
	AllItems.Append(EquipmentItems);
	AllItems.Append(ConsumableItems);
	AllItems.Append(OtherItems);
	return AllItems;
}

void UInventoryComponent::GetValidItemsByType(EItemType Type, TArray<UInventoryItem*>& OutItems) const
{
	OutItems.Reset();

	const TArray<UInventoryItem*>& Src = GetItemsByType(Type);
	for (UInventoryItem* Item : Src)
	{
		if (IsValid(Item))
		{
			OutItems.Add(Item);
		}
	}
}


