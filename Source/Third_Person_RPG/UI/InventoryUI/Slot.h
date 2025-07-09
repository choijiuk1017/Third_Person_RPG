// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/UI/InventoryUI/CustomWidget.h"
#include "Slot.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	ST_InventoryEquipment UMETA(DisplayName = "Equipment"),
	ST_InventoryConsumable UMETA(DisplayName = "Consumable"),
	ST_InventoryOther UMETA(DisplayName = "Other")
};

UCLASS()
class THIRD_PERSON_RPG_API USlot : public UCustomWidget
{
	GENERATED_BODY()

public:
    /** 슬롯 타입 설정 */
    void SetType(ESlotType InType);

    /** 슬롯 상태 업데이트 */
    void UpdateSlot();

    /** 인덱스 설정 */
    void SetIndex(int32 InIndex) { SlotIndex = InIndex; }

    void SetItem(UInventoryItem* NewItem);

    void ClearItem();

    UPROPERTY(VisibleAnywhere, Category = "Slot")
    int32 SlotIndex;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Slot")
    ESlotType SlotType;

    UPROPERTY(meta = (BindWidget))
    class UImage* IMG_Item;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TXT_Quantity;

    UPROPERTY(EditAnywhere, Category = "Slot")
    TObjectPtr<UTexture2D> DefaultTexture;
};
