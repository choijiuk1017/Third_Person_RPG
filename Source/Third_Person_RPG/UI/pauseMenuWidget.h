// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "pauseMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UpauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// ESC 입력 받기
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_ReturnToTitle;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_QuitGame;

	UFUNCTION()
	void OnReturnToTitleClicked();

	UFUNCTION()
	void OnQuitGameClicked();

public:
	// 열고/닫기
	void Open(APlayerController* InPC);
	void Close();

private:
	UPROPERTY()
	APlayerController* OwningPC = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pause")
	FName TitleLevelName = TEXT("Start");
};
