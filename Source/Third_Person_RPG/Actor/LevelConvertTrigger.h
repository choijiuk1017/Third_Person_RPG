// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LevelConvertTrigger.generated.h"

class UBoxComponent;
class UUserWidget;

UCLASS()
class THIRD_PERSON_RPG_API ALevelConvertTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelConvertTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Level")
	FName NextLevelName;

	UPROPERTY()
	ULevelStreamingDynamic* LoadedLevel;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoadingScreen;

	UUserWidget* LoadingScreenWidget;

	bool bTransitioning = false;

	// 로컬이 아니라 멤버로 보관
	FTimerHandle LevelTransitionTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
