// Fill out your copyright notice in the Description page of Project Settings
#include "Third_Person_RPG/Inventory/InventoryComponent.h"

#include "Engine/AssetManager.h"
#include "Third_Person_RPG/Data/ItemData/TPRItemData.h"
#include "Third_Person_RPG/Inventory/InventoryItem.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	bWantsInitializeComponent = true;

	MaxInventoryNum = 30;
	MaxItemNum = 99;

	EquipmentItems.Init(nullptr, MaxInventoryNum);
	ConsumableItems.Init(nullptr, MaxInventoryNum);
	OtherItems.Init(nullptr, MaxInventoryNum);
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitInventory();
}

void UInventoryComponent::InitInventory()
{
	// 애셋 매니저 불러오기
	UAssetManager& Manager = UAssetManager::Get();

	// 애셋 아이디 리스트 받아오기
	TArray<FPrimaryAssetId> Assets;
	// * 태그 정보를 넘겨줘서 동일한 태그를 가진 애셋들의 목록을 배열로 반환받음
	Manager.GetPrimaryAssetIdList(TEXT("TPRItemData"), Assets);

	if (Assets.Num() > 0)
	{
		TMap<int32, TPair<FName, int32>> InventoryEquipmentArray;
		TMap<int32, TPair<FName, int32>> InventoryConstableArray;
		// TODO : 세이브 파일에서 데이터 읽어오기 (현재는 테스트 용도)
		{
			InventoryEquipmentArray.Add(1, { TEXT("DA_Sword"), 1 });
		}

		for (const auto& InvItem : InventoryEquipmentArray)
		{
			// 특정 아이템 키 생성
			FPrimaryAssetId Key;
			Key.PrimaryAssetType = TEXT("TPRItemData");
			Key.PrimaryAssetName = InvItem.Value.Key;

			if (Assets.Contains(Key))
			{
				// 아이템 생성
				UInventoryItem* NewItem = NewObject<UInventoryItem>();
				if (NewItem)
				{
					FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Assets.FindByKey(Key)[0]));
					if (AssetPtr.IsPending())
					{
						AssetPtr.LoadSynchronous();
					}
					UTPRItemData* ItemData = Cast<UTPRItemData>(AssetPtr.Get());
					if (ItemData)
					{
						NewItem->ItemData = ItemData;
						NewItem->ItemQuantity = InvItem.Value.Value;
						// 아이템 넣기
						EquipmentItems[InvItem.Key] = NewItem;
					}
				}
			}
		}

		for (const auto& InvItem : InventoryConstableArray)
		{
			// 특정 아이템 키 생성
			FPrimaryAssetId Key;
			Key.PrimaryAssetType = TEXT("TPRItemData");
			Key.PrimaryAssetName = InvItem.Value.Key;

			if (Assets.Contains(Key))
			{
				// 아이템 생성
				UInventoryItem* NewItem = NewObject<UInventoryItem>();
				if (NewItem)
				{
					FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Assets.FindByKey(Key)[0]));
					if (AssetPtr.IsPending())
					{
						AssetPtr.LoadSynchronous();
					}
					UTPRItemData* ItemData = Cast<UTPRItemData>(AssetPtr.Get());
					if (ItemData)
					{
						NewItem->ItemData = ItemData;
						NewItem->ItemQuantity = InvItem.Value.Value;
						// 아이템 넣기
						ConsumableItems[InvItem.Key] = NewItem;
					}
				}
			}
		}
	}
}

bool UInventoryComponent::AddItem(FName InItemName, int32 InItemQuantity, int32& OutItemQuantity)
{
	// 성공적으로 추가했는지에 대한 결과 반환용 변수
	bool bIsResult = false;

	// 애셋 매니저 불러오기
	UAssetManager& Manager = UAssetManager::Get();

	// 애셋 아이디 리스트 받아오기
	TArray<FPrimaryAssetId> Assets;
	// * 태그 정보를 넘겨줘서 동일한 태그를 가진 애셋들의 목록을 배열로 반환받음
	Manager.GetPrimaryAssetIdList(TEXT("TPRItemData"), Assets);

	// 특정 아이템 키 생성
	FPrimaryAssetId Key;
	Key.PrimaryAssetType = TEXT("TPRItemData");
	Key.PrimaryAssetName = InItemName;

	// 해당 키의 애셋이 존재한다면?
	if (Assets.Contains(Key))
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Data"));

		// 아이템 생성
		UInventoryItem* NewItem = NewObject<UInventoryItem>();
		if (NewItem)
		{
			FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Assets.FindByKey(Key)[0]));
			if (AssetPtr.IsPending())
			{
				AssetPtr.LoadSynchronous();
			}
			UTPRItemData* ItemData = Cast<UTPRItemData>(AssetPtr.Get());
			if (ItemData)
			{
				NewItem->ItemData = ItemData;
				NewItem->ItemQuantity = InItemQuantity;
			}
			else
			{
				return bIsResult;
			}
		}

		// 이미 해당 아이템이 존재하는지 체크하기 (포션, 기타)
		if (NewItem->ItemData->ItemType != EItemType::IT_Weapon)
		{
			// 타입에 따라 해당 인벤토리에 저장하기
			switch (NewItem->ItemData->ItemType)
			{
			case EItemType::IT_Potion:
				for (UInventoryItem* Item : ConsumableItems)
				{
					if (IsValid(Item))
					{
						// 이름이 동일한 아이템이 있는지 체크하기
						if (Item->ItemData->ItemName == NewItem->ItemData->ItemName)
						{
							// 최대 수량 체크하기
							if (Item->ItemQuantity + NewItem->ItemQuantity > MaxItemNum)
							{
								// 최대 수량까지 채운 후 남은 값으로 설정하기
								NewItem->ItemQuantity = Item->ItemQuantity + NewItem->ItemQuantity - MaxItemNum;
								Item->ItemQuantity = MaxItemNum;
								break;
							}
							else
							{
								// 수량을 더하고 종료하기
								Item->ItemQuantity += NewItem->ItemQuantity;
								OnChangeInven.Broadcast();
								return true;
							}
						}
					}
				}
				break;
			}
		}

		// 해당 아이템 타입의 인벤토리의 빈 칸 찾아 데이터 추가하기
		int32 Index = 0;
		switch (NewItem->ItemData->ItemType)
		{
		case EItemType::IT_Weapon:
			for (UInventoryItem* Item : EquipmentItems)
			{
				if (!IsValid(Item))
				{
					OtherItems[Index] = NewItem;
					bIsResult = true;
					OnChangeInven.Broadcast();
					break;
				}

				Index++;
			}
			break;
		case EItemType::IT_Potion:
			for (UInventoryItem* Item : ConsumableItems)
			{
				if (!IsValid(Item))
				{
					OtherItems[Index] = NewItem;
					bIsResult = true;
					OnChangeInven.Broadcast();
					break;
				}

				Index++;
			}
			break;
		}

		if (!bIsResult)
		{
			OutItemQuantity = NewItem->ItemQuantity;
			return bIsResult;
		}
		else
		{
			return bIsResult;
		}
	}

	OutItemQuantity = InItemQuantity;
	return bIsResult;
}

void UInventoryComponent::UseItem(int32 InSlotIndex, ESlotType InventoryType)
{
	// 해당 인벤토리 슬롯에 아이템이 존재하는지 체크하고 사용하기
	switch (InventoryType)
	{
	case ESlotType::ST_InventoryConsumable:
		if (ConsumableItems.IsValidIndex(InSlotIndex) && IsValid(ConsumableItems[InSlotIndex]))
		{
			// 수량을 줄여줍니다.
			ConsumableItems[InSlotIndex]->ItemQuantity--;
			// 아이템을 사용합니다. TODO : 플레이어에서 작업하기
			UE_LOG(LogTemp, Warning, TEXT("ConsumableItem Use"));
			// 수량이 0 이하라면 소멸시켜줍니다.
			if (ConsumableItems[InSlotIndex]->ItemQuantity <= 0)
			{
				RemoveItem(InSlotIndex, InventoryType);
			}
		}
		break;
	}
}

void UInventoryComponent::SwapItem(int32 InPrevIndex, int32 InCurrentIndex, ESlotType InPrevSlotType, ESlotType InCurrentSlotType)
{
	// 슬롯 타입이 같은 경우 교환해주도록 합니다.
	if (InPrevSlotType == InCurrentSlotType)
	{
		switch (InCurrentSlotType)
		{
		case ESlotType::ST_InventoryEquipment:
			// 해당 슬롯의 아이템이 유효한지 체크합니다.
			if (EquipmentItems.IsValidIndex(InPrevIndex) && EquipmentItems.IsValidIndex(InCurrentIndex))
			{
				// 교체 후 이벤트를 호출합니다.
				EquipmentItems.Swap(InPrevIndex, InCurrentIndex);
				OnChangeInven.Broadcast();
			}
			break;

		case ESlotType::ST_InventoryConsumable:
			// 해당 슬롯의 아이템이 유효한지 체크합니다.
			if (ConsumableItems.IsValidIndex(InPrevIndex) && ConsumableItems.IsValidIndex(InCurrentIndex))
			{
				// 교체 후 이벤트를 호출합니다.
				ConsumableItems.Swap(InPrevIndex, InCurrentIndex);
				OnChangeInven.Broadcast();
			}
			break;

		case ESlotType::ST_InventoryOther:
			// 해당 슬롯의 아이템이 유효한지 체크합니다.
			if (OtherItems.IsValidIndex(InPrevIndex) && OtherItems.IsValidIndex(InCurrentIndex))
			{
				// 교체 후 이벤트를 호출합니다.
				OtherItems.Swap(InPrevIndex, InCurrentIndex);
				OnChangeInven.Broadcast();
			}
			break;
		}
	}
}

void UInventoryComponent::SortItem(ESlotType InSlotType)
{
	// 슬롯 타입에 따라 정렬합니다.
	switch (InSlotType)
	{
	case ESlotType::ST_InventoryEquipment:
		Algo::Sort(EquipmentItems,
			[](const TObjectPtr<UInventoryItem>& A, const TObjectPtr<UInventoryItem>& B)
			{
				// 해당 요소가 nullptr이라면 뒤로 배치합니다
				if (!IsValid(A) && !IsValid(B))
					return false;
				else if (!IsValid(A))
					return false;
				else if (!IsValid(B))
					return true;

				// 동일한 이름의 아이템이라면 수량 순으로 배치합니다.
				if (A->ItemData->ItemName == B->ItemData->ItemName)
				{
					return A->ItemQuantity > B->ItemQuantity;
				}
				// 다른 이름의 아이템이라면 아이템 이름 순으로 배치합니다.
				else
				{
					return A->ItemData->ItemName < B->ItemData->ItemName;
				}
			}
		);
		OnChangeInven.Broadcast();
		break;

	case ESlotType::ST_InventoryConsumable:
		Algo::Sort(ConsumableItems,
			[](const TObjectPtr<UInventoryItem>& A, const TObjectPtr<UInventoryItem>& B)
			{
				// 해당 요소가 nullptr이라면 뒤로 배치합니다
				if (!IsValid(A) && !IsValid(B))
					return false;
				else if (!IsValid(A))
					return false;
				else if (!IsValid(B))
					return true;

				// 동일한 이름의 아이템이라면 수량 순으로 배치합니다.
				if (A->ItemData->ItemName == B->ItemData->ItemName)
				{
					return A->ItemQuantity > B->ItemQuantity;
				}
				// 다른 이름의 아이템이라면 아이템 이름 순으로 배치합니다.
				else
				{
					return A->ItemData->ItemName < B->ItemData->ItemName;
				}
			}
		);
		OnChangeInven.Broadcast();
		break;

	case ESlotType::ST_InventoryOther:
		Algo::Sort(OtherItems,
			[](const TObjectPtr<UInventoryItem>& A, const TObjectPtr<UInventoryItem>& B)
			{
				// 해당 요소가 nullptr이라면 뒤로 배치합니다
				if (!IsValid(A) && !IsValid(B))
					return false;
				else if (!IsValid(A))
					return false;
				else if (!IsValid(B))
					return true;

				// 동일한 이름의 아이템이라면 수량 순으로 배치합니다.
				if (A->ItemData->ItemName == B->ItemData->ItemName)
				{
					return A->ItemQuantity > B->ItemQuantity;
				}
				// 다른 이름의 아이템이라면 아이템 이름 순으로 배치합니다.
				else
				{
					return A->ItemData->ItemName < B->ItemData->ItemName;
				}
			}
		);
		OnChangeInven.Broadcast();
		break;
	}
}

void UInventoryComponent::RemoveItem(int32 InSlotIndex, ESlotType InventoryType)
{
	// 해당 인벤토리 슬롯의 유효성을 체크하고 소멸시켜줍니다.
	switch (InventoryType)
	{
	case ESlotType::ST_InventoryEquipment:
		if (EquipmentItems.IsValidIndex(InSlotIndex) && IsValid(EquipmentItems[InSlotIndex]))
		{
			EquipmentItems[InSlotIndex] = nullptr;
		}
		break;

	case ESlotType::ST_InventoryConsumable:
		if (ConsumableItems.IsValidIndex(InSlotIndex) && IsValid(ConsumableItems[InSlotIndex]))
		{
			ConsumableItems[InSlotIndex] = nullptr;
		}
		break;

	case ESlotType::ST_InventoryOther:
		if (OtherItems.IsValidIndex(InSlotIndex) && IsValid(OtherItems[InSlotIndex]))
		{
			OtherItems[InSlotIndex] = nullptr;
		}
		break;
	}
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

