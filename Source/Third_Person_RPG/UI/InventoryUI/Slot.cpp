// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InventoryUI/Slot.h"

#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Kismet/GameplayStatics.h"
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
    InventoryItem = NewItem; //이게 없으면 드래그 시 nullptr 됨

    if (!IMG_Item || !TXT_Quantity) return;

    if (NewItem && NewItem->ItemData && NewItem->GetItemTexture())
    {
        IMG_Item->SetBrushFromTexture(NewItem->GetItemTexture());
        IMG_Item->SetVisibility(ESlateVisibility::Visible);

        TXT_Quantity->SetText(FText::AsNumber(NewItem->Quantity));
        TXT_Quantity->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        IMG_Item->SetBrushFromTexture(DefaultTexture);
        IMG_Item->SetVisibility(ESlateVisibility::Visible);

        TXT_Quantity->SetText(FText::FromString(TEXT("")));
        TXT_Quantity->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void USlot::ClearItem()
{
    if (IMG_Item && DefaultTexture)
    {
        IMG_Item->SetBrushFromTexture(DefaultTexture);
        IMG_Item->SetVisibility(ESlateVisibility::Hidden);
    }

    if (TXT_Quantity)
    {
        TXT_Quantity->SetText(FText::FromString(TEXT("")));
    }
}

FReply USlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    UE_LOG(LogTemp, Warning, TEXT("Mouse Button Down"));

    if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
    {
        // 우클릭 시 무기 해제 시도
        if (SlotType == ESlotType::ST_EquipWeapon && InventoryItem)
        {
            UnequipItem();
            return FReply::Handled();
        }
    }

    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void USlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Drag Detected"));


    if (!InventoryItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("InventoryItem is nullptr"));
        return;
    }

    UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
    DragOp->Payload = InventoryItem;
    DragOp->DefaultDragVisual = this; // 혹은 복제된 슬롯 이미지
    OutOperation = DragOp;
}

bool USlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (SlotType != ESlotType::ST_EquipWeapon) return false;

    if (UInventoryItem* DroppedItem = Cast<UInventoryItem>(InOperation->Payload))
    {
        if (DroppedItem->ItemData->ItemType == EItemType::IT_Weapon)
        {
            SetItem(DroppedItem);  // 슬롯에 표시

            if (OwningActor && OwningActor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
            {
                UE_LOG(LogTemp, Warning, TEXT("Weapon Equiped"))
                IInventoryInterface::Execute_EquipWeapon(OwningActor, DroppedItem);
            }


            return true;
        }
    }

    return false;
}


void USlot::UnequipItem()
{

    if (InventoryItem)
    {
        if (OwningActor && OwningActor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
        {
            IInventoryInterface::Execute_UnEquipWeapon(OwningActor, InventoryItem);
        }

        InventoryItem = nullptr;
        ClearItem();
    }
}

