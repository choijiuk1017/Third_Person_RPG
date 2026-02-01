// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

class USphereComponent;
class APlayerCharacter;

UCLASS()
class THIRD_PERSON_RPG_API ANPC : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USphereComponent* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	TArray<FText> FirstEncounterLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	TArray<FText> RepeatLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	bool bHasMetPlayer = false;

	int32 CurrentLineIndex = 0;
	bool bIsTalking = false;

	const TArray<FText>* ActiveLines = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FText HelpText;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class UStaticMeshComponent* ShieldMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartTalk(APlayerCharacter* Player);

	void AdvanceTalk(APlayerCharacter* Player);

	void EndTalk(APlayerCharacter* Player);
};
