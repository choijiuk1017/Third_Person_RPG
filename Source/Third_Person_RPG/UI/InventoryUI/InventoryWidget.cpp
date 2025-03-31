// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/InventoryUI/InventoryWidget.h"


#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Third_Person_RPG/UI/InventoryUI/Slot.h"
#include "Third_Person_RPG/Interface/InventoryInterface.h"

void UInventoryWidget::NativeConstruct()
{

}


void UInventoryWidget::Init()
{
	SetType(ESlotType::ST_InventoryEquipment);

	if (BTN_Equipment)
		BTN_Equipment->OnClicked.AddDynamic(this, &UInventoryWidget::SetEquipmentType);
	if (BTN_Consumable)
		BTN_Consumable->OnClicked.AddDynamic(this, &UInventoryWidget::SetConsumableType);
	if (BTN_Other)
		BTN_Other->OnClicked.AddDynamic(this, &UInventoryWidget::SetOtherType);
	if (BTN_SortItem)
		BTN_SortItem->OnClicked.AddDynamic(this, &UInventoryWidget::SortItem);

	Slots.Init(nullptr, 30); // 슬롯 30개 초기화

	if (SlotContainer && SlotClass)
	{
		for (int32 Index = 0; Index < 30; ++Index)
		{
			// 슬롯 위젯 생성
			USlot* NewSlot = CreateWidget<USlot>(GetWorld(), SlotClass);
			if (NewSlot)
			{
				NewSlot->SlotIndex = Index;
				NewSlot->SetOwningActor(OwningActor);
				NewSlot->SetType(InventorySlotType);
				NewSlot->Init();

				// UI에 추가 (UniformGridPanel 기준)
				int32 Row = Index / 5;
				int32 Col = Index % 5;
				SlotContainer->AddChildToUniformGrid(NewSlot, Row, Col);

				Slots[Index] = NewSlot;
			}
		}
	}
}

void UInventoryWidget::UpdateInventorySlot()
{
	// 슬롯을 현재 인벤토리 타입으로 업데이트 합니다.
	for (const auto& InvSlot : Slots)
	{
		InvSlot->SetType(InventorySlotType);
		InvSlot->UpdateSlot();
	}
}



void UInventoryWidget::SetEquipmentType()
{
	SetType(ESlotType::ST_InventoryEquipment);
}

void UInventoryWidget::SetConsumableType()
{
	SetType(ESlotType::ST_InventoryConsumable);
}

void UInventoryWidget::SetOtherType()
{
	SetType(ESlotType::ST_InventoryOther);
}

void UInventoryWidget::SortItem()
{
	// 인벤토리 컴포넌트 내부 아이템 정렬 함수를 호출합니다.
	IInventoryInterface* InvPlayer = Cast<IInventoryInterface>(OwningActor);
	if (InvPlayer)
	{
		InvPlayer->GetInventoryComponent()->SortItem(InventorySlotType);
	}
}

void UInventoryWidget::SetType(ESlotType Type)
{
	// 현재 인벤토리의 타입을 변경합니다.
	InventorySlotType = Type;
	// 타입이 변경되었으므로 슬롯을 다시 업데이트 해줍니다.
	UpdateInventorySlot();
}