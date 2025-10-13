// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/TutorialWidgetTrigger.h"

#include "Components/BoxComponent.h"
#include "UObject/UnrealType.h"  
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"


// Sets default values
ATutorialWidgetTrigger::ATutorialWidgetTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATutorialWidgetTrigger::OnTriggerOverlap);

}

// Called when the game starts or when spawned
void ATutorialWidgetTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATutorialWidgetTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATutorialWidgetTrigger::OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{

	}
}