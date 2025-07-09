// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InventoryUI/Slot.h"

#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USlot::SetType(ESlotType InType)
{
    SlotType = InType;
    UpdateSlot(); // 타입에 따라 상태 업데이트
}

void USlot::UpdateSlot()
{
    // 예시: 초기 상태에서는 비어 있음
    if (IMG_Item && DefaultTexture)
    {
        IMG_Item->SetBrushFromTexture(DefaultTexture);
    }

    if (TXT_Quantity)
    {
        TXT_Quantity->SetText(FText::FromString(TEXT("")));
    }
}

void USlot::SetItem(UInventoryItem* NewItem)
{
    if (!NewItem || !IMG_Item)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetItem 실패: NewItem 또는 IMG_Item이 nullptr"));
        return;
    }

    if (!NewItem->ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetItem 실패: ItemData가 nullptr"));
        return;
    }

    UTexture2D* ItemTexture = NewItem->GetItemTexture();

    if (ItemTexture)
    {
        IMG_Item->SetBrushFromTexture(ItemTexture);
        IMG_Item->SetVisibility(ESlateVisibility::Visible);
        TXT_Quantity->SetText(FText::AsNumber(NewItem->Quantity));

        UE_LOG(LogTemp, Warning, TEXT("SetItem 성공: 아이템 [%s], 수량 [%d], 텍스처 [%s]"),
            *NewItem->ItemData->ItemName, NewItem->Quantity, *ItemTexture->GetName());
    }
    else
    {
        IMG_Item->SetBrushFromTexture(DefaultTexture);
        IMG_Item->SetVisibility(ESlateVisibility::Hidden);
        TXT_Quantity->SetText(FText::FromString(TEXT("")));

        UE_LOG(LogTemp, Warning, TEXT("SetItem 실패: [%s] 텍스처 없음 -> 기본 텍스처 처리"), *NewItem->ItemData->ItemName);
    }
}

void USlot::ClearItem()
{
    if (IMG_Item && DefaultTexture)
    {
        IMG_Item->SetBrushFromTexture(DefaultTexture);
        IMG_Item->SetVisibility(ESlateVisibility::Visible);
    }

    if (TXT_Quantity)
    {
        TXT_Quantity->SetText(FText::FromString(TEXT("")));
    }
}

