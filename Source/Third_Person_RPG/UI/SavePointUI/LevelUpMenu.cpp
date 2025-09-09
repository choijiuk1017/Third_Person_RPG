// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/LevelUpMenu.h"

#include "Components/Button.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

void ULevelUpMenu::NativeConstruct()
{
    Super::NativeConstruct();
    Init();

}


void ULevelUpMenu::Init()
{
    UpdateCurrentStats();
}


void ULevelUpMenu::UpdateCurrentStats()
{
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        const auto& Attr = PC->CharacterAttributes;
        const auto& Derived = PC->DerivedStats;
        const auto& Combat = PC->CombatStats;


        if (CurrentLevelText)       CurrentLevelText->SetText(FText::AsNumber(PC->Level));
        if (CurrentVigorText)       CurrentVigorText->SetText(FText::AsNumber(Attr.Vigor));
        if (CurrentMindText)        CurrentMindText->SetText(FText::AsNumber(Attr.Mind));
        if (CurrentEnduranceText)   CurrentEnduranceText->SetText(FText::AsNumber(Attr.Endurance));
        if (CurrentStrengthText)    CurrentStrengthText->SetText(FText::AsNumber(Attr.Strength));
        if (CurrentDexterityText)   CurrentDexterityText->SetText(FText::AsNumber(Attr.Dexterity));
        if (CurrentIntelligenceText)CurrentIntelligenceText->SetText(FText::AsNumber(Attr.Intelligence));
        if (CurrentFaithText)       CurrentFaithText->SetText(FText::AsNumber(Attr.Faith));
        if (CurrentArcaneText)      CurrentArcaneText->SetText(FText::AsNumber(Attr.Arcane));

        if (Text_HP) Text_HP->SetText(FText::AsNumber(Derived.MaxHP));

        if (Text_FP) Text_FP->SetText(FText::AsNumber(Derived.MaxFP));

        if (Text_Stamina) Text_Stamina->SetText(FText::AsNumber(Derived.MaxStamina));

        if (Text_Weight)
        {
            const float MaxW = Derived.MaxEquipLoad;

            Text_Weight->SetText(FText::AsNumber(MaxW));
        }
    }
}