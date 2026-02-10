// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

#include "BossSequenceTrigger.generated.h"

class ALevelSequenceActor;
class ASkeletalMeshActor;

UCLASS()
class THIRD_PERSON_RPG_API ABossSequenceTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossSequenceTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TSubclassOf<AActor> CombatBossClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TSubclassOf<AActor> HiddenBossClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutscene", meta = (AllowedClasses = "LevelSequenceActor"))
	TSoftObjectPtr<ALevelSequenceActor> SequenceActor;

	void SetAllUIVisible(bool bVisible);

	UPROPERTY(EditAnywhere, Category = "Cutscene")
	TArray<ASkeletalMeshActor*> SkeletalMeshActorsToDestroy;

	UPROPERTY(EditAnywhere, Category = "Cutscene")
	TArray<AStaticMeshActor*> StaticMeshActorsToDestroy;

	UPROPERTY(EditAnywhere, Category = "BossRoom")
	TArray<AStaticMeshActor*> BossRoomBlock;

	bool bHasPlayed = false;

	UFUNCTION(BlueprintCallable)
	void OnSequenceEnd();

	void SetPlayerInputEnabled(bool bEnabled);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	FName BossID;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	bool bHasClearedBoss = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bIsHiddenBoss = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutscene", meta = (AllowedClasses = "LevelSequenceActor"))
	TSoftObjectPtr<ALevelSequenceActor> HiddenBossSequenceActor;

	UPROPERTY(EditInstanceOnly, Category = "BGM")
	AActor* AmbientBGMActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "BGM")
	UAudioComponent* AmbientBGMComp = nullptr;  

	UPROPERTY(EditAnywhere, Category = "BGM")
	USoundBase* BossBGM = nullptr;

	UPROPERTY()
	UAudioComponent* BossBGMComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "BGM")
	bool bResumeAmbientAfterBoss = true;

	UPROPERTY(EditAnywhere, Category = "BGM")
	float FadeOutTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "BGM")
	float FadeInTime = 0.5f;

	void StartBossBGM();

	void StopBossBGMAndResumeAmbient();
};
