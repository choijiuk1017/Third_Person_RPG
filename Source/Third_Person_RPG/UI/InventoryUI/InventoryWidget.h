
#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/UI/InventoryUI/CustomWidget.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/UI/InventoryUI/Slot.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"
#include "Components/Button.h" 
#include "Styling/SlateColor.h"
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

    void UpdateStatPanel();

    void UpdateWeaponInfo(UWeaponItemData* WeaponData);

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
    class UImage* Image_WeaponIcon;

    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Level;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Vigor;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Mind;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Endurance;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Strength;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Dexterity;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Intelligence;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Faith;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Arcane;

    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_HP;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_FP;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Stamina;

    // 무기 기본 정보
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_WeaponName;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_WeaponType;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_WeaponWeight;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_WeaponFP;

    // 요구 능력치
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Require_Strength;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Require_Dexterity;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Require_Intelligence;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Require_Faith;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Require_Arcane;

    // 스탯 보정
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Scaling_Strength;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Scaling_Dexterity;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Scaling_Intelligence;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Scaling_Faith;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Scaling_Arcane;

    UPROPERTY(meta = (BindWidget))
    USlot* EquippedWeaponSlot;

    FSlateColor RedColor = FSlateColor(FLinearColor::Red);
    FSlateColor WhiteColor = FSlateColor(FLinearColor::White);

};
