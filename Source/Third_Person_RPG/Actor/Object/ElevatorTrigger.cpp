// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/Object/ElevatorTrigger.h"
#include "Third_Person_RPG/Actor/Object/ElevatorPlatform.h"
#include "Components/BoxComponent.h"

// Sets default values
AElevatorTrigger::AElevatorTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AElevatorTrigger::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AElevatorTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AElevatorTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AElevatorTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Elevator)
	{
		Elevator->ReverseDirection();
	}
}

