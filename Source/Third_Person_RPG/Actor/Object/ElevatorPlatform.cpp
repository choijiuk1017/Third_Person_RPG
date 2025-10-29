// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/Object/ElevatorPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
AElevatorPlatform::AElevatorPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ElevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorMesh"));
	RootComponent = ElevatorMesh;
}

// Called when the game starts or when spawned
void AElevatorPlatform::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AElevatorPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsMoving) return;

	FVector NewLocation = GetActorLocation();
	NewLocation += (bMovingUp ? FVector::UpVector : -FVector::UpVector) * MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}

void AElevatorPlatform::ReverseDirection()
{
	bMovingUp = !bMovingUp;
}

