// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InventoryUI/Slot.h"

#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USlot::NativeConstruct()
{

}

void USlot::Init()
{
	// 개별 함수를 연동해 맵에 저장해주도록 합니다.
	SlotUpdateActions.Add(ESlotType::ST_InventoryEquipment, FUpdateSlotDelegateWrapper(FOnUpdateSlotDelegate::CreateUObject(this, &USlot::UpdateEquipmentSlot)));
	SlotUpdateActions.Add(ESlotType::ST_InventoryConsumable, FUpdateSlotDelegateWrapper(FOnUpdateSlotDelegate::CreateUObject(this, &USlot::UpdateConsumableSlot)));
	SlotUpdateActions.Add(ESlotType::ST_InventoryOther, FUpdateSlotDelegateWrapper(FOnUpdateSlotDelegate::CreateUObject(this, &USlot::UpdateOtherSlot)));

	// 초기화 시 업데이트를 진행합니다.
	UpdateSlot();
}

void USlot::SetType(ESlotType InType)
{
	SlotType = InType;

	// 타입 설정 후 자동 업데이트 하고 싶다면 이 줄도 추가
	UpdateSlot();
}

void USlot::UpdateSlot()
{
	SlotUpdateActions[SlotType].SlotDelegate.ExecuteIfBound();
}


void USlot::UpdateEquipmentSlot()
{
	IInventoryInterface* InvPlayer = Cast<IInventoryInterface>(OwningActor);

	if (InvPlayer)
	{
		// 인벤토리(Equipment)를 가져옵니다.
		TArray<UInventoryItem*> InventoryItems = InvPlayer->GetInventoryComponent()->GetEquipmentItems();

		// 현재 Slot의 인덱스가 유효한지 체크합니다.
		if (InventoryItems.IsValidIndex(SlotIndex))
		{
			// 해당 슬롯에 아이템이 존재하는지 확인합니다.
			if (IsValid(InventoryItems[SlotIndex]))
			{
				// 존재하는 경우 아이템의 텍스쳐와 수량을 반영해주도록 합니다. (장비는 수량 표시 X)
				IMG_Item->SetBrushFromTexture(InventoryItems[SlotIndex]->ItemData->ItemTexture);
				TXT_Quantity->SetText(FText::FromString(TEXT("")));
			}
			else
			{
				// 존재하지 않는 경우 빈 칸으로 표시합니다.
				IMG_Item->SetBrushFromTexture(DefaultTexture);
				TXT_Quantity->SetText(FText::FromString(TEXT("")));
			}
		}
	}
}
void USlot::UpdateConsumableSlot()
{
	IInventoryInterface* InvPlayer = Cast<IInventoryInterface>(OwningActor);

	if (InvPlayer)
	{
		// 인벤토리(Consumable)를 가져옵니다.
		TArray<UInventoryItem*> InventoryItems = InvPlayer->GetInventoryComponent()->GetConsumableItems();

		// 현재 Slot의 인덱스가 유효한지 체크합니다.
		if (InventoryItems.IsValidIndex(SlotIndex))
		{
			// 해당 슬롯에 아이템이 존재하는지 확인합니다.
			if (IsValid(InventoryItems[SlotIndex]))
			{
				// 존재하는 경우 아이템의 텍스쳐와 수량을 반영해주도록 합니다. (소비는 수량 표시 O)
				IMG_Item->SetBrushFromTexture(InventoryItems[SlotIndex]->ItemData->ItemTexture);
				TXT_Quantity->SetText(FText::FromString(FString::Printf(TEXT("%d"), InventoryItems[SlotIndex]->ItemQuantity)));
			}
			else
			{
				// 존재하지 않는 경우 빈 칸으로 표시합니다.
				IMG_Item->SetBrushFromTexture(DefaultTexture);
				TXT_Quantity->SetText(FText::FromString(TEXT("")));
			}
		}
	}
}

void USlot::UpdateOtherSlot()
{
	IInventoryInterface* InvPlayer = Cast<IInventoryInterface>(OwningActor);

	if (InvPlayer)
	{
		// 인벤토리(Consumable)를 가져옵니다.
		TArray<UInventoryItem*> InventoryItems = InvPlayer->GetInventoryComponent()->GetOtherItems();

		// 현재 Slot의 인덱스가 유효한지 체크합니다.
		if (InventoryItems.IsValidIndex(SlotIndex))
		{
			// 해당 슬롯에 아이템이 존재하는지 확인합니다.
			if (IsValid(InventoryItems[SlotIndex]))
			{
				// 존재하는 경우 아이템의 텍스쳐와 수량을 반영해주도록 합니다. (기타는 수량 표시 O)
				IMG_Item->SetBrushFromTexture(InventoryItems[SlotIndex]->ItemData->ItemTexture);
				TXT_Quantity->SetText(FText::FromString(FString::Printf(TEXT("%d"), InventoryItems[SlotIndex]->ItemQuantity)));
			}
			else
			{
				// 존재하지 않는 경우 빈 칸으로 표시합니다.
				IMG_Item->SetBrushFromTexture(DefaultTexture);
				TXT_Quantity->SetText(FText::FromString(TEXT("")));
			}
		}
	}
}


