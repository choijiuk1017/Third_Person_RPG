// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"                   // 드래그-드롭용
#include "Blueprint/WidgetBlueprintLibrary.h"              // DetectDragIfPressed
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/UI/InventoryUI/CustomWidget.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"     
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"   
#include "Slot.generated.h"

UENUM(BlueprintType)
enum class ESlotType : uint8
{
	ST_InventoryEquipment UMETA(DisplayName = "Equipment"),
	ST_InventoryConsumable UMETA(DisplayName = "Consumable"),
	ST_InventoryOther UMETA(DisplayName = "Other"),

    ST_EquipWeapon UMETA(DisplayName = "Equip Weapon")
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

    // 마우스 클릭 시 드래그 감지용
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // 드래그 시작 시 호출
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    void UnequipItem();

    void RefreshAmountText();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Slot")
    ESlotType SlotType;

    UPROPERTY(meta = (BindWidget))
    class UImage* IMG_Item;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TXT_Quantity;

    UPROPERTY(EditAnywhere, Category = "Slot")
    TObjectPtr<UTexture2D> DefaultTexture;

    UPROPERTY()
    UInventoryItem* InventoryItem;

  
};
