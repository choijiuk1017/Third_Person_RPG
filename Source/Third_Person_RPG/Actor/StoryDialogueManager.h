// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoryDialogueManager.generated.h"

class UStoryDialogueWidget;

UCLASS()
class THIRD_PERSON_RPG_API AStoryDialogueManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoryDialogueManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|UI")
	TSubclassOf<UStoryDialogueWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|Data")
	TArray<FText> SlideTexts;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|UI")
	int32 ZOrder = 999999;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|UI")
	bool bAutoCreateOnShow = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TObjectPtr<UStoryDialogueWidget> WidgetInstance;

	void EnsureWidget();

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void ShowDialogue(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void ShowWidgetOnly();

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void HideDialogue();

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	bool IsValidIndex(int32 Index) const;

};
