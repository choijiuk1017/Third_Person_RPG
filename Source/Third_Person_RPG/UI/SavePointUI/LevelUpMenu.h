// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h" 
#include "Input/Reply.h"       // FReply
#include "Input/Events.h"
#include "LevelUpMenu.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API ULevelUpMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init();

	void UpdateCurrentStats();

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentLevelText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentCurrencyText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* LevelUpText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* RemainingCurrencyText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* NeedCurrencyText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentVigorText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentMindText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentEnduranceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentStrengthText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentDexterityText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentIntelligenceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentFaithText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CurrentArcaneText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* VigorText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MindText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* EnduranceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StrengthText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* DexterityText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* IntelligenceText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* FaithText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* ArcaneText;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_HP;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_FP;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Stamina;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Weight;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Attack;


	UPROPERTY(meta = (BindWidget))
	class UButton* DecisionButton;


	int32 SelectedIndex = 0;

	// 각 스탯별 미리보기 증가치(>=0)
	TArray<int32> Pending; // 길이 8

	// 현재/미리보기 텍스트 배열 (인덱스 매핑 고정)
	TArray<UTextBlock*> CurrentTexts; // CurrentVigorText ...
	TArray<UTextBlock*> PreviewTexts; // VigorText ...

	// 하이라이트 색
	FLinearColor NormalColor = FLinearColor::White;
	FLinearColor SelectedColor = FLinearColor(1.f, 0.84f, 0.f, 1.f); // 골드 톤

	/** ===== 내부 유틸 ===== */
	void BuildWidgetArrays();       // 배열 매핑 구성
	void RefreshPreview();          // 미리보기/비용/레벨 미리보기 갱신
	void UpdateRowHighlight();      // 선택 행 강조
	void MoveSelection(int32 Delta);// 위/아래 이동
	void AdjustStat(int32 Index, int32 Delta); // 좌/우로 +1/-1

	int32 SumPendingLevels() const;               // Pending 총합
	int32 CalculateRequiredCurrency() const;      // Pending 기준 필요 재화
	int32 RequiredCurrencyForLevels(int32 N) const;
	int32 GetCostForLevel(int32 TargetLevel) const; // ★ 비용 공식 교체 지점

	void ApplyPendingToPlayer();    // ‘결정’ 적용

	/** ===== 결정 버튼 콜백 ===== */
	UFUNCTION() void OnDecisionClicked();

	UFUNCTION() void OnExitClicked();
	void CloseMenuToGameOnly();
};
