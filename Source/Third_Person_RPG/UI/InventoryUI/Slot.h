// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/UI/InventoryUI/CustomWidget.h"
#include "Slot.generated.h"

DECLARE_DELEGATE(FOnUpdateSlotDelegate);

// 래퍼 구조체를 선언
USTRUCT(BlueprintType)
struct FUpdateSlotDelegateWrapper
{
    GENERATED_BODY()

    FUpdateSlotDelegateWrapper() {}
    FUpdateSlotDelegateWrapper(const FOnUpdateSlotDelegate& InSlotDelegate) : SlotDelegate(InSlotDelegate) {}

    FOnUpdateSlotDelegate SlotDelegate;
};

UCLASS()
class THIRD_PERSON_RPG_API USlot : public UCustomWidget
{
	GENERATED_BODY()



protected:
    virtual void NativeConstruct() override;

public:
    /** 슬롯 초기화 함수 */
    void Init();
    /** 슬롯의 타입 설정 함수 */
    void SetType(ESlotType InType);
    /** 슬롯 업데이트 함수 */
    void UpdateSlot();


public:
    /** 현재 슬롯의 타입 */
    UPROPERTY(VisibleAnywhere, Category = "Slot")
    ESlotType SlotType;

    /** 슬롯에 표시될 아이템 이미지 */
    UPROPERTY(VisibleAnywhere, Category = "Slot", meta = (BindWidget = "true"))
    TObjectPtr<class UImage> IMG_Item;

    /** 슬롯에 표시될 아이템 수량 */
    UPROPERTY(VisibleAnywhere, Category = "Slot", meta = (BindWidget = "true"))
    TObjectPtr<class UTextBlock> TXT_Quantity;

    /** 현재 슬롯의 인덱스 */
    UPROPERTY(EditAnywhere, Category = "Slot")
    int32 SlotIndex;

protected:
    /** 개별 슬롯 업데이트 함수들 */
    void UpdateEquipmentSlot();
    void UpdateConsumableSlot();
    void UpdateOtherSlot();

    /** 빈 슬롯에 적용할 기본 텍스처 */
    UPROPERTY(EditAnywhere, Category = "Slot")
    TObjectPtr<class UTexture2D> DefaultTexture;

    UPROPERTY()
    TMap<ESlotType, FUpdateSlotDelegateWrapper> SlotUpdateActions;
	
};
