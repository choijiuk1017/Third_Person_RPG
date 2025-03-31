// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnChangedInventoryDelegate);


UENUM(BlueprintType)
enum class ESlotType : uint8
{
	ST_InventoryEquipment,
	ST_InventoryConsumable,
	ST_InventoryOther,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THIRD_PERSON_RPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnChangedInventoryDelegate OnChangeInven;

	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void InitializeComponent() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UInventoryItem>> EquipmentItems;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UInventoryItem>> ConsumableItems;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UInventoryItem>> OtherItems;

	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 MaxInventoryNum;


	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 MaxItemNum;

	FORCEINLINE TArray<TObjectPtr<class UInventoryItem>> GetEquipmentItems() { return EquipmentItems; }
	FORCEINLINE TArray<TObjectPtr<class UInventoryItem>> GetConsumableItems() { return ConsumableItems; }
	FORCEINLINE TArray<TObjectPtr<class UInventoryItem>> GetOtherItems() { return OtherItems; }

	bool AddItem(FName InItemName, int32 InItemQuantity, int32& OutItemQuantity);

	void UseItem(int32 InSlotIndex, ESlotType InventoryType);

	void SwapItem(int32 InPrevIndex, int32 InCurrentIndex, ESlotType InPrevSlotType, ESlotType InCurrentSlotType);

	void SortItem(ESlotType InSlotType);

protected:
	void InitInventory();

	void RemoveItem(int32 InSlotIndex, ESlotType InventoryType);
};
