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
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/Data/ItemData/WeaponItemData.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BTN_Equipment)
        BTN_Equipment->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickEquipment);

    if (BTN_Consumable)
        BTN_Consumable->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickConsumable);

    if (BTN_Other)
        BTN_Other->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickOther);

    //if (BTN_Sort)
    //    BTN_Sort->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickSort);

    EquippedWeaponSlot->SetType(ESlotType::ST_EquipWeapon);
    EquippedWeaponSlot->SetIndex(0);

    EquippedWeaponSlot->OwningActor = GetOwningPlayerPawn();


    Init();

    if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        if (PC->InventoryComponent && PC->InventoryComponent->EquippedWeaponItem)
        {
            EquippedWeaponSlot->SetItem(PC->InventoryComponent->EquippedWeaponItem);
        }
    }
}

void UInventoryWidget::OnClickEquipment()
{
    InventorySlotType = ESlotType::ST_InventoryEquipment;
    UpdateInventorySlot();
}

void UInventoryWidget::OnClickConsumable()
{
    InventorySlotType = ESlotType::ST_InventoryConsumable;
    UpdateInventorySlot();
}

void UInventoryWidget::OnClickOther()
{
    InventorySlotType = ESlotType::ST_InventoryOther;
    UpdateInventorySlot();
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
            UpdateStatPanel();

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

            NewSlot->OwningActor = GetOwningPlayerPawn();

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
        return EItemType::IT_Consumable;
    case ESlotType::ST_InventoryOther:
        return EItemType::IT_Other;
    default:
        return EItemType::IT_None;
    }
}

void UInventoryWidget::UpdateStatPanel()
{
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        const auto& Attr = PC->CharacterAttributes;
        const auto& Derived = PC->DerivedStats;
        const auto& Combat = PC->CombatStats;


        if (Text_Level)       Text_Level->SetText(FText::AsNumber(PC->Level)); // 레벨은 따로 변수로 있는 경우
        if (Text_Vigor)       Text_Vigor->SetText(FText::AsNumber(Attr.Vigor));
        if (Text_Mind)        Text_Mind->SetText(FText::AsNumber(Attr.Mind));
        if (Text_Endurance)   Text_Endurance->SetText(FText::AsNumber(Attr.Endurance));
        if (Text_Strength)    Text_Strength->SetText(FText::AsNumber(Attr.Strength));
        if (Text_Dexterity)   Text_Dexterity->SetText(FText::AsNumber(Attr.Dexterity));
        if (Text_Intelligence)Text_Intelligence->SetText(FText::AsNumber(Attr.Intelligence));
        if (Text_Faith)       Text_Faith->SetText(FText::AsNumber(Attr.Faith));
        if (Text_Arcane)      Text_Arcane->SetText(FText::AsNumber(Attr.Arcane));

        if (Text_HP) Text_HP->SetText(FText::Format(FText::FromString("{0} / {1}"),
            FText::AsNumber(Combat.CurrentHP), FText::AsNumber(Derived.MaxHP)));

        if (Text_FP) Text_FP->SetText(FText::Format(FText::FromString("{0} / {1}"),
            FText::AsNumber(Combat.CurrentFP), FText::AsNumber(Derived.MaxFP)));

        if (Text_Stamina) Text_Stamina->SetText(FText::Format(FText::FromString("{0} / {1}"),
            FText::AsNumber(Combat.CurrentStamina), FText::AsNumber(Derived.MaxStamina)));

        if (Text_Weight)
        {
            const float CurrentW = GetCurrentEquipWeight();   // 헬퍼 사용 중이면 그대로
            const float MaxW = Derived.MaxEquipLoad;

            // 상태 문자열
            const FString StateStr = GetWeightStateString(CurrentW, MaxW);

            // 보기 좋은 한 자리 소수 표시 (Printf 이용)
            const FString WeightStr = FString::Printf(TEXT("%.1f / %.1f (%s)"),
                CurrentW, MaxW, *StateStr);

            Text_Weight->SetText(FText::FromString(WeightStr));

            // (선택) 상태에 따라 색상 변화
            if (StateStr == TEXT("Heavy"))
            {
                Text_Weight->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
            }
            else if (StateStr == TEXT("Light"))
            {
                Text_Weight->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.9f, 0.6f)));
            }
            else // 보통
            {
                Text_Weight->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            }
        }
    }
}

void UInventoryWidget::UpdateWeaponInfo(UWeaponItemData* WeaponData)
{
    if (!WeaponData) return;

    const FWeaponStatData& Stats = WeaponData->WeaponStats;

    if (Image_WeaponIcon)
    {
        if (WeaponData->ItemTexture)
        {
            Image_WeaponIcon->SetBrushFromTexture(WeaponData->ItemTexture);
            Image_WeaponIcon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            Image_WeaponIcon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    // 무기 텍스트 기본 출력
    Text_WeaponName->SetText(Stats.WeaponName);
    Text_WeaponType->SetText(Stats.WeaponType);
    Text_WeaponWeight->SetText(FText::AsNumber(Stats.Weight));
    Text_WeaponFP->SetText(FText::AsNumber(Stats.FPCost));

    // 현재 플레이어 능력치와 비교
    APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn());
    if (!PC) return;

    const auto& Attr = PC->CharacterAttributes;

    auto SetStatText = [](UTextBlock* TextBlock, int32 Value, bool bExceeds)
        {
            if (!TextBlock) return;
            TextBlock->SetText(FText::AsNumber(Value));
            FSlateColor Color = bExceeds ? FSlateColor(FLinearColor::Red) : FSlateColor(FLinearColor::White);
            TextBlock->SetColorAndOpacity(Color);
        };

    // 요구 능력치 비교 및 색상 적용
    SetStatText(Text_Require_Strength, Stats.RequiredStrength, Attr.Strength < Stats.RequiredStrength);
    SetStatText(Text_Require_Dexterity, Stats.RequiredDexterity, Attr.Dexterity < Stats.RequiredDexterity);
    SetStatText(Text_Require_Intelligence, Stats.RequiredIntelligence, Attr.Intelligence < Stats.RequiredIntelligence);
    SetStatText(Text_Require_Faith, Stats.RequiredFaith, Attr.Faith < Stats.RequiredFaith);
    SetStatText(Text_Require_Arcane, Stats.RequiredArcane, Attr.Arcane < Stats.RequiredArcane);

    if (Text_Scaling_Strength) Text_Scaling_Strength->SetText(FText::FromString(Stats.StrengthScaling));
    if (Text_Scaling_Dexterity) Text_Scaling_Dexterity->SetText(FText::FromString(Stats.DexterityScaling));
    if (Text_Scaling_Intelligence) Text_Scaling_Intelligence->SetText(FText::FromString(Stats.IntelligenceScaling));
    if (Text_Scaling_Faith) Text_Scaling_Faith->SetText(FText::FromString(Stats.FaithScaling));
    if (Text_Scaling_Arcane) Text_Scaling_Arcane->SetText(FText::FromString(Stats.ArcaneScaling));
}

float UInventoryWidget::GetCurrentEquipWeight() const
{
    if (const APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        if (const UInventoryComponent* Inv = PC->InventoryComponent)
        {
            if (const UInventoryItem* Equipped = Inv->EquippedWeaponItem)
            {
                if (const UWeaponItemData* WData = Cast<UWeaponItemData>(Equipped->ItemData))
                {
                    return WData->WeaponStats.Weight;
                }
            }
        }
    }
    return 0.f;
}

FString UInventoryWidget::GetWeightStateString(float Current, float Max) const
{
    const float Half = Max * 0.5f;

    if (Current <= Half - 5.f)   return TEXT("Light");
    if (Current < Half + 5.f)   return TEXT("Normal");
    return TEXT("Heavy");
}

//void UInventoryWidget::OnClickSort()
//{
//    if (IInventoryInterface* Interface = Cast<IInventoryInterface>(OwningActor))
//    {
//        if (UInventoryComponent* Inventory = Interface->GetInventoryComponent())
//        {
//            Inventory->SortItemsByName(ConvertSlotTypeToItemType(InventorySlotType));
//        }
//    }
//}




