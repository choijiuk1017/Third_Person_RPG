
#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/UI/InventoryUI/CustomWidget.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/UI/InventoryUI/Slot.h"
#include "Components/Button.h" 
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UInventoryWidget : public UCustomWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    void Init();

    UFUNCTION()
    void UpdateInventorySlot();



private:
    void CreateSlots(int32 NumSlots = 30);

    // 슬롯 그리드 컨테이너
    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* SlotContainer;

    // 슬롯 클래스 (블루프린트에서 지정)
    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<class USlot> SlotClass;

    // 생성된 슬롯들을 저장하는 배열
    UPROPERTY()
    TArray<TObjectPtr<class USlot>> SlotList;

    // 슬롯 타입 (기본값: 장비)
    ESlotType InventorySlotType = ESlotType::ST_InventoryEquipment;

    EItemType ConvertSlotTypeToItemType(ESlotType SlotType);

    // 슬롯 타입 변경 함수
    UFUNCTION()
    void OnClickEquipment();

    UFUNCTION()
    void OnClickConsumable();

    UFUNCTION()
    void OnClickOther();

    /*UFUNCTION()
    void OnClickSort();*/

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Equipment;

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Consumable;

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Other;

    UPROPERTY(meta = (BindWidget))
    class UButton* BTN_Sort;

    UPROPERTY(meta = (BindWidget))
    USlot* EquippedWeaponSlot;
};
