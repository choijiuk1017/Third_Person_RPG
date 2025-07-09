// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InventoryUI/InventoryWidget.h"


#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"
#include "Third_Person_RPG/UI/InventoryUI/Slot.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Init();
}

void UInventoryWidget::Init()
{
    if (IInventoryInterface* Interface = Cast<IInventoryInterface>(OwningActor))
    {
        if (UInventoryComponent* Inventory = Interface->GetInventoryComponent())
        {
            // 슬롯 생성
            CreateSlots(30);

            // 델리게이트 바인딩
            Inventory->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::UpdateInventorySlot);
            UE_LOG(LogTemp, Warning, TEXT("Delegate Binding"));

            // 초기화 시 슬롯도 한 번 그려줌
            UpdateInventorySlot();

        }
    }
}

void UInventoryWidget::CreateSlots(int32 NumSlots)
{
    if (!SlotContainer || !SlotClass) return;

    SlotList.Empty(); // 혹시 이전에 생성된 슬롯이 있다면 제거

    for (int32 i = 0; i < NumSlots; ++i)
    {
        USlot* NewSlot = CreateWidget<USlot>(this, SlotClass);
        if (NewSlot)
        {
            NewSlot->SetType(InventorySlotType);
            NewSlot->SetIndex(i);

            SlotList.Add(NewSlot);
            SlotContainer->AddChildToUniformGrid(NewSlot, i / 5, i % 5);
        }
    }
}

void UInventoryWidget::UpdateInventorySlot()
{
    UE_LOG(LogTemp, Warning, TEXT("Slot Update"));

    if (!SlotContainer || SlotList.Num() == 0) return;

    if (IInventoryInterface* Interface = Cast<IInventoryInterface>(OwningActor))
    {
        if (UInventoryComponent* Inventory = Interface->GetInventoryComponent())
        {
            const TArray<UInventoryItem*>& Items = Inventory->GetItemsByType(ConvertSlotTypeToItemType(InventorySlotType));
            UE_LOG(LogTemp, Warning, TEXT("item num: %d"), Items.Num());

            for (int32 i = 0; i < SlotList.Num(); ++i)
            {
                if (!SlotList[i]) continue;

                if (i < Items.Num() && IsValid(Items[i]))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Slot %d Set Item: %s"), i, *Items[i]->ItemData->ItemName);
                    SlotList[i]->SetItem(Items[i]);
                }
                else
                {
                    SlotList[i]->ClearItem();
                }
            }
        }
    }
}

EItemType UInventoryWidget::ConvertSlotTypeToItemType(ESlotType SlotType)
{
	switch (SlotType)
	{
	case ESlotType::ST_InventoryEquipment:
		return EItemType::IT_Weapon;
	case ESlotType::ST_InventoryConsumable:
		return EItemType::IT_Potion;
	default:
		return EItemType::IT_Weapon;
	}
}


