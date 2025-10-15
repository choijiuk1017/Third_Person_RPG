// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialWidgetTrigger.generated.h"

class UTutorialWidget;

UCLASS()
class THIRD_PERSON_RPG_API ATutorialWidgetTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialWidgetTrigger();

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Tutorial")
	TSubclassOf<UTutorialWidget> TutorialWidgetClass;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Tutorial")
	FName TriggerID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bHasPlayed = false;
};
