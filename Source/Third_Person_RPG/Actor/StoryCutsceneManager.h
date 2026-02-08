// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoryCutsceneManager.generated.h"

class UTexture2D;
class UStoryCutsceneWidget;
class ASkeletalMeshActor;

UCLASS()
class THIRD_PERSON_RPG_API AStoryCutsceneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoryCutsceneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|UI")
	TSubclassOf<UStoryCutsceneWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|Data")
	TArray<TObjectPtr<UTexture2D>> SlideImages;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|Data")
	TArray<FText> SlideTexts;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|UI")
	int32 ZOrder = 999999;

	UPROPERTY(EditAnywhere, Category = "StoryCutscene|UI")
	bool bAutoCreateOnShow = true;

private:
	UPROPERTY()
	TObjectPtr<UStoryCutsceneWidget> WidgetInstance;

	void EnsureWidget();

public:
	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void ShowSlide(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void ShowWidgetOnly();

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void HideWidget();

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	bool IsValidIndex(int32 Index) const;

	UPROPERTY(EditAnywhere, Category = "Cutscene")
	TArray<ASkeletalMeshActor*> SkeletalMeshActorsToDestroy;

	UPROPERTY(EditAnywhere, Category = "Cutscene")
	TArray<AStaticMeshActor*> StaticMeshActorsToDestroy;

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void DestroySkeletalMesh();

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void SetAllUIVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void SetPlayerInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "StoryCutscene")
	void OpenTutorial();
};
