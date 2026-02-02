// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InventoryUI/Slot.h"

#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
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

        RefreshAmountText();
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
        // 슬롯에 아이템이 있고 무기 아이템일 경우
        if (InventoryItem && InventoryItem->ItemData->ItemType == EItemType::IT_Weapon)
        {
            if (UWeaponItemData* WeaponData = Cast<UWeaponItemData>(InventoryItem->ItemData))
            {
                if (OwningActor && OwningActor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
                {
                    IInventoryInterface::Execute_ShowWeaponInfo(OwningActor, WeaponData);
                }
            }
        }

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
            // 무기 데이터 가져오기
            if (UWeaponItemData* WeaponData = Cast<UWeaponItemData>(DroppedItem->ItemData))
            {
                // 플레이어 캐릭터 캐스팅
                APlayerCharacter* Player = Cast<APlayerCharacter>(OwningActor);
                if (!Player) return false;

                const FCharacterAttributes& Attributes = Player->CharacterAttributes;
                const FWeaponStatData& ReqStats = WeaponData->WeaponStats;

                // 능력치 조건 비교
                if (Attributes.Strength < ReqStats.RequiredStrength ||
                    Attributes.Dexterity < ReqStats.RequiredDexterity ||
                    Attributes.Intelligence < ReqStats.RequiredIntelligence ||
                    Attributes.Faith < ReqStats.RequiredFaith ||
                    Attributes.Arcane < ReqStats.RequiredArcane)
                {
                    UE_LOG(LogTemp, Warning, TEXT("장착 실패: 요구 능력치 부족"));
                    // 추후: UI 메시지 출력 가능
                    return false;
                }

                // 조건 통과: 무기 장착
                SetItem(DroppedItem);  // 슬롯에 표시

                if (OwningActor && OwningActor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Weapon Equipped"));
                    IInventoryInterface::Execute_EquipWeapon(OwningActor, DroppedItem);
                }

                return true;
            }
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
        InventoryItem->bEquipped = false;
        InventoryItem = nullptr;
        ClearItem();
    }
}

void USlot::RefreshAmountText()
{
    if (!TXT_Quantity)
        return;

    if (!InventoryItem)
    {
        TXT_Quantity->SetText(FText::GetEmpty());
        TXT_Quantity->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    const EItemType Type = InventoryItem->GetItemType();

    if (Type == EItemType::IT_Weapon)
    {
        const FString EnhanceStr = FString::Printf(TEXT("+%d"), InventoryItem->EnhanceLevel);
        TXT_Quantity->SetText(FText::FromString(EnhanceStr));
        TXT_Quantity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
    else
    {
        const int32 Qty = InventoryItem->Quantity;
        if (Qty > 1)
        {
            TXT_Quantity->SetText(FText::AsNumber(Qty));
            TXT_Quantity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        else
        {
            TXT_Quantity->SetText(FText::GetEmpty());
            TXT_Quantity->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}
