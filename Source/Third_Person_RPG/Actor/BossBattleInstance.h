// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossBattleInstance.generated.h"

class ABossCharacter;

UCLASS()
class THIRD_PERSON_RPG_API ABossBattleInstance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossBattleInstance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Boss Battle")
	ABossCharacter* BossRef;

	UPROPERTY(EditAnywhere, Category = "BossRoom")
	TArray<AStaticMeshActor*> BossRoomBlock;

	UFUNCTION()
	void OnBossDiedHandler();

	UPROPERTY(EditAnywhere, Category = "Boss")
	FName BossActorTag = "Boss";

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
