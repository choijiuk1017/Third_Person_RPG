// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/LevelPreloadTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelPreloadTrigger::ALevelPreloadTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelPreloadTrigger::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ALevelPreloadTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelPreloadTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALevelPreloadTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->ActorHasTag("Player")) return;

	UGameplayStatics::LoadStreamLevel(GetWorld(), TargetLevelName, true, false, FLatentActionInfo());

}
