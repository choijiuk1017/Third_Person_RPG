// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/SavePointUI/LevelUpMenu.h"

#include "Components/Button.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void ULevelUpMenu::NativeConstruct()
{
    Super::NativeConstruct();

	// 키 입력 받기
	SetIsFocusable(true);
	SetKeyboardFocus();

	Init();

	if (DecisionButton)
	{
		DecisionButton->OnClicked.AddDynamic(this, &ULevelUpMenu::OnDecisionClicked);
	}

}


void ULevelUpMenu::Init()
{
	BuildWidgetArrays();

	Pending.Empty();
	Pending.Init(0, 8);

	SelectedIndex = 0;

	UpdateCurrentStats(); // 현재값 표시 리셋
	RefreshPreview();     // 미리보기 초기 표시
	UpdateRowHighlight(); // 첫 행 하이라이트
}

void ULevelUpMenu::BuildWidgetArrays()
{
    CurrentTexts.Empty();
    CurrentTexts.Add(CurrentVigorText);
    CurrentTexts.Add(CurrentMindText);
    CurrentTexts.Add(CurrentEnduranceText);
    CurrentTexts.Add(CurrentStrengthText);
    CurrentTexts.Add(CurrentDexterityText);
    CurrentTexts.Add(CurrentIntelligenceText);
    CurrentTexts.Add(CurrentFaithText);
    CurrentTexts.Add(CurrentArcaneText);

    PreviewTexts.Empty();
    PreviewTexts.Add(VigorText);
    PreviewTexts.Add(MindText);
    PreviewTexts.Add(EnduranceText);
    PreviewTexts.Add(StrengthText);
    PreviewTexts.Add(DexterityText);
    PreviewTexts.Add(IntelligenceText);
    PreviewTexts.Add(FaithText);
    PreviewTexts.Add(ArcaneText);
}

void ULevelUpMenu::UpdateCurrentStats()
{
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        const auto& Attr = PC->CharacterAttributes;
        const auto& Derived = PC->DerivedStats;
        const auto& Combat = PC->CombatStats;


        if (CurrentLevelText)       CurrentLevelText->SetText(FText::AsNumber(PC->Level));
        if (LevelUpText)            LevelUpText->SetText(FText::AsNumber(PC->Level));

        if (CurrentCurrencyText)    CurrentCurrencyText->SetText(FText::AsNumber(PC->Currency));
        if (RemainingCurrencyText)  RemainingCurrencyText->SetText(FText::AsNumber(PC->Currency));

        if (CurrentVigorText)       CurrentVigorText->SetText(FText::AsNumber(Attr.Vigor));
        if (CurrentMindText)        CurrentMindText->SetText(FText::AsNumber(Attr.Mind));
        if (CurrentEnduranceText)   CurrentEnduranceText->SetText(FText::AsNumber(Attr.Endurance));
        if (CurrentStrengthText)    CurrentStrengthText->SetText(FText::AsNumber(Attr.Strength));
        if (CurrentDexterityText)   CurrentDexterityText->SetText(FText::AsNumber(Attr.Dexterity));
        if (CurrentIntelligenceText)CurrentIntelligenceText->SetText(FText::AsNumber(Attr.Intelligence));
        if (CurrentFaithText)       CurrentFaithText->SetText(FText::AsNumber(Attr.Faith));
        if (CurrentArcaneText)      CurrentArcaneText->SetText(FText::AsNumber(Attr.Arcane));

        if (VigorText)              VigorText->SetText(FText::AsNumber(Attr.Vigor));
        if (MindText)               MindText->SetText(FText::AsNumber(Attr.Mind));
        if (EnduranceText)          EnduranceText->SetText(FText::AsNumber(Attr.Endurance));
        if (StrengthText)           StrengthText->SetText(FText::AsNumber(Attr.Strength));
        if (DexterityText)          DexterityText->SetText(FText::AsNumber(Attr.Dexterity));
        if (IntelligenceText)       IntelligenceText->SetText(FText::AsNumber(Attr.Intelligence));
        if (FaithText)              FaithText->SetText(FText::AsNumber(Attr.Faith));
        if (ArcaneText)             ArcaneText->SetText(FText::AsNumber(Attr.Arcane));

        if (Text_HP) Text_HP->SetText(FText::AsNumber(Derived.MaxHP));

        if (Text_FP) Text_FP->SetText(FText::AsNumber(Derived.MaxFP));

        if (Text_Stamina) Text_Stamina->SetText(FText::AsNumber(Derived.MaxStamina));

        if (Text_Weight)
        {
            const float MaxW = Derived.MaxEquipLoad;

            Text_Weight->SetText(FText::AsNumber(MaxW));
        }

		if (Text_Attack)  Text_Attack->SetText(FText::AsNumber(Combat.AttackPower));
    }
}

void ULevelUpMenu::RefreshPreview()
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		const auto& Attr = PC->CharacterAttributes;

		const int32 Values[8] = {
			Attr.Vigor + Pending[0],
			Attr.Mind + Pending[1],
			Attr.Endurance + Pending[2],
			Attr.Strength + Pending[3],
			Attr.Dexterity + Pending[4],
			Attr.Intelligence + Pending[5],
			Attr.Faith + Pending[6],
			Attr.Arcane + Pending[7]
		};

		for (int32 i = 0; i < 8; ++i)
		{
			if (PreviewTexts.IsValidIndex(i) && PreviewTexts[i])
			{
				PreviewTexts[i]->SetText(FText::AsNumber(Values[i]));
			}
		}

		const int32 PendingLevels = SumPendingLevels();
		const int32 NeedCurrency = CalculateRequiredCurrency();
		const int32 Remain = FMath::Max(0, PC->Currency - NeedCurrency);

		if (LevelUpText)           LevelUpText->SetText(FText::AsNumber(PC->Level + PendingLevels));
		if (NeedCurrencyText)      NeedCurrencyText->SetText(FText::AsNumber(NeedCurrency));
		if (RemainingCurrencyText) RemainingCurrencyText->SetText(FText::AsNumber(Remain));
	}
}

void ULevelUpMenu::UpdateRowHighlight()
{
	for (int32 i = 0; i < PreviewTexts.Num(); ++i)
	{
		if (UTextBlock* TB = PreviewTexts[i])
		{
			TB->SetColorAndOpacity(FSlateColor(NormalColor));
		}
	}
	if (PreviewTexts.IsValidIndex(SelectedIndex) && PreviewTexts[SelectedIndex])
	{
		PreviewTexts[SelectedIndex]->SetColorAndOpacity(FSlateColor(SelectedColor));
	}
}

FReply ULevelUpMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Up)
	{
		MoveSelection(-1);
		return FReply::Handled();
	}
	if (Key == EKeys::Down)
	{
		MoveSelection(+1);
		return FReply::Handled();
	}
	if (Key == EKeys::Right)
	{
		AdjustStat(SelectedIndex, +1);
		return FReply::Handled();
	}
	if (Key == EKeys::Left)
	{
		AdjustStat(SelectedIndex, -1);
		return FReply::Handled();
	}
	// 방향키만으로 “결정”까지 하고 싶으시면 엔터/스페이스를 허용합니다.
	if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept || Key == EKeys::SpaceBar)
	{
		OnDecisionClicked();
		return FReply::Handled();
	}
	if (Key == EKeys::X)
	{
		OnExitClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void ULevelUpMenu::MoveSelection(int32 Delta)
{
	SelectedIndex = (SelectedIndex + Delta + 8) % 8;
	UpdateRowHighlight();
}

int32 ULevelUpMenu::SumPendingLevels() const
{
	int32 Sum = 0;
	for (int32 v : Pending) Sum += v;
	return Sum;
}

int32 ULevelUpMenu::GetCostForLevel(int32 TargetLevel) const
{
	const int32 Base = 100;
	const int32 Step = 50;
	return Base + (TargetLevel - 1) * Step;
}

int32 ULevelUpMenu::RequiredCurrencyForLevels(int32 N) const
{
	if (N <= 0) return 0;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		int32 Sum = 0;
		for (int32 i = 1; i <= N; ++i)
		{
			Sum += GetCostForLevel(PC->Level + i);
		}
		return Sum;
	}
	return 0;
}

int32 ULevelUpMenu::CalculateRequiredCurrency() const
{
	return RequiredCurrencyForLevels(SumPendingLevels());
}

void ULevelUpMenu::AdjustStat(int32 Index, int32 Delta)
{
	if (!Pending.IsValidIndex(Index)) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		const int32 CurrSum = SumPendingLevels();

		if (Delta > 0)
		{
			const int32 NextSum = CurrSum + 1;
			const int32 Need = RequiredCurrencyForLevels(NextSum);
			if (Need <= PC->Currency)
			{
				Pending[Index] += 1; 
			}
		}
		else if (Delta < 0)
		{
			if (Pending[Index] > 0)
			{
				Pending[Index] -= 1;
			}
		}

		RefreshPreview();
		UpdateRowHighlight();
	}
}

void ULevelUpMenu::ApplyPendingToPlayer()
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		const int32 PendingLevels = SumPendingLevels();
		const int32 NeedCurrency = CalculateRequiredCurrency();

		if (PendingLevels <= 0) return;
		if (PC->Currency < NeedCurrency) return;

		auto& Attr = PC->CharacterAttributes;
		Attr.Vigor += Pending[0];
		Attr.Mind += Pending[1];
		Attr.Endurance += Pending[2];
		Attr.Strength += Pending[3];
		Attr.Dexterity += Pending[4];
		Attr.Intelligence += Pending[5];
		Attr.Faith += Pending[6];
		Attr.Arcane += Pending[7];

		PC->Level += PendingLevels;

		PC->SpentCurrencyOnStats += NeedCurrency;

		PC->SpendCurrency(NeedCurrency);

		PC->RecalculateStatsAfterLevelUp(false);

		Pending.Init(0, 8);
		UpdateCurrentStats();
		RefreshPreview();
		UpdateRowHighlight();
	}
}

void ULevelUpMenu::OnDecisionClicked()
{
	ApplyPendingToPlayer();
}

void ULevelUpMenu::OnExitClicked()
{
	CloseMenuToGameOnly();
}

void ULevelUpMenu::CloseMenuToGameOnly()
{
	Pending.Init(0, 8);

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;

		APawn* Pawn = PC->GetPawn();
		if (Pawn)
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
			if (PlayerCharacter)
			{
				PlayerCharacter->EndInteractSavePoint(); // 호출

				PlayerCharacter->SavePointMenuInstance = nullptr;
			}
		}

	}

	RemoveFromParent();
}