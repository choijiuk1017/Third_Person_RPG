// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CurrentEquipedWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UCurrentEquipedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 무기 아이콘 갱신 (아이콘 없으면 nullptr 전달) */
	UFUNCTION(BlueprintCallable, Category = "CurrentEquipped")
	void UpdateWeaponIcon(UTexture2D* InIcon);

	/** 포션 아이콘/개수 갱신 (아이콘 없으면 nullptr 가능) */
	UFUNCTION(BlueprintCallable, Category = "CurrentEquipped")
	void UpdatePotion(UTexture2D* InIcon, int32 InCount);

	/** 무기 칸 비우기 */
	UFUNCTION(BlueprintCallable, Category = "CurrentEquipped")
	void ClearWeapon();

	/** 포션 칸 비우기(개수도 0으로) */
	UFUNCTION(BlueprintCallable, Category = "CurrentEquipped")
	void ClearPotion();

protected:
	virtual void NativeConstruct() override;

	/** UMG 바인딩 */
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* PotionIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PotionCountText;

	/** 빈칸 기본 텍스처(선택) */
	UPROPERTY(EditAnywhere, Category = "Style")
	TObjectPtr<UTexture2D> EmptySlotTexture = nullptr;

private:
	void SetImageFromTexture(UImage* Target, UTexture2D* Texture);
	void SetCountText(int32 Count);
};
