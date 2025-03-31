
#pragma once

#include "CoreMinimal.h"
#include "Third_Person_RPG/UI/InventoryUI/CustomWidget.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
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
	// 인벤토리 초기화 함수
	void Init();
	// 인벤토리 아이템 슬롯 업데이트 함수
	void UpdateInventorySlot();

private:
	UFUNCTION()
	void SetEquipmentType();

	UFUNCTION()
	void SetConsumableType();

	UFUNCTION()
	void SetOtherType();

	UFUNCTION()
	void SortItem();

	// Main
public:
	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UButton> BTN_MainButton;

	// Header
public:
	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UButton> BTN_Equipment;

	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UButton> BTN_Consumable;

	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UButton> BTN_Other;

	// Tail
public:
	UPROPERTY(meta = (BindWidget = "true"))
	TObjectPtr<class UButton> BTN_SortItem;


private:
	// 현재 인벤토리의 타입을 지정하기 위한 함수
	void SetType(ESlotType Type);

	// 슬롯들을 저장하기 위한 배열
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TObjectPtr<class USlot>> Slots;

	// 슬롯을 담을 부모 위젯
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotContainer;


	// 슬롯 클래스 (블루프린트에서 지정 가능)
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class USlot> SlotClass;

	// 현재 인벤토리의 타입 (장비 or 소비 or 기타)
	ESlotType InventorySlotType;
};
