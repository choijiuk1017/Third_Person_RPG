// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/LevelConvertTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"


// Sets default values
ALevelConvertTrigger::ALevelConvertTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelConvertTrigger::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ALevelConvertTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelConvertTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALevelConvertTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->ActorHasTag("Player")) return;

	ULevelStreaming* StreamedLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), NextLevelName);

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (UInventoryComponent* Inventory = PC->FindComponentByClass<UInventoryComponent>())
		{
			if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				GI->CacheInventory(Inventory->GetAllItems());
				GI->RegisterPlayerStatFromPlayer(PC);
			}
		}
	}

	
	if (StreamedLevel && StreamedLevel->IsLevelLoaded())
	{
		UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
		UE_LOG(LogTemp, Warning, TEXT("Trying to convert level: %s"), *NextLevelName.ToString());
		return;
	}

	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
	UE_LOG(LogTemp, Warning, TEXT("Trying to convert level: %s"), *NextLevelName.ToString());
}

