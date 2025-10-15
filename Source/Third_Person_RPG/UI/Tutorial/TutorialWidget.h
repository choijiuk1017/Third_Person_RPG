// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Delegates/DelegateCombinations.h"
#include "TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class THIRD_PERSON_RPG_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTutorialFinishedDelegate, FName, TutorialID);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FTutorialFinishedDelegate OnTutorialFinished;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	FName TutorialID;

	UFUNCTION(BlueprintCallable)
	void NotifyTutorialFinished();

	UFUNCTION(BlueprintCallable, Category="Tutorial")
	void CloseWidget();

	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
