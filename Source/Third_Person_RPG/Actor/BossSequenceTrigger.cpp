// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/BossSequenceTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UnrealType.h"  
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
// Sets default values
ABossSequenceTrigger::ABossSequenceTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossSequenceTrigger::OnTriggerOverlap);

}

// Called when the game starts or when spawned
void ABossSequenceTrigger::BeginPlay()
{
	Super::BeginPlay();
	for (AStaticMeshActor* BlockActor : BossRoomBlock)
	{
		if (IsValid(BlockActor))
		{
			BlockActor->SetActorHiddenInGame(true);
			BlockActor->SetActorEnableCollision(false);
		}
	}
}

// Called every frame
void ABossSequenceTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossSequenceTrigger::OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

	if (bHasPlayed) return;


	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		bHasPlayed = true;

		SetPlayerInputEnabled(false);
		SetAllUIVisible(false);


		// 컷씬 재생
		if (SequenceActor)
		{

			ULevelSequencePlayer* Player = SequenceActor->GetSequencePlayer();
			if (Player)
			{
				Player->OnFinished.AddDynamic(this, &ABossSequenceTrigger::OnSequenceEnd);
				Player->Play();
			}
		}
	}
}

void ABossSequenceTrigger::OnSequenceEnd()
{
	FVector SpawnLocation = SkeletalMeshActorsToDestroy[0]->GetActorLocation();
	SpawnLocation.Z += 100.0f; // Z축 위로 100만큼 올리기
	SpawnLocation.X += 500.0f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);

	for (AStaticMeshActor* MeshActor : StaticMeshActorsToDestroy)
	{
		if (IsValid(MeshActor))
		{
			MeshActor->Destroy();
		}
	}

	for (ASkeletalMeshActor* SkeletalActor : SkeletalMeshActorsToDestroy)
	{
		if (IsValid(SkeletalActor))
		{
			SkeletalActor->Destroy();
		}
	}

	for (AStaticMeshActor* BlockActor : BossRoomBlock)
	{
		if (IsValid(BlockActor))
		{
			BlockActor->SetActorHiddenInGame(false);
			BlockActor->SetActorEnableCollision(true);
		}
	}

	if (CombatBossClass && SpawnTransform.IsValid())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* SpawnedBoss = GetWorld()->SpawnActor<AActor>(CombatBossClass, SpawnTransform, Params);
	}

	SetPlayerInputEnabled(true);
	SetAllUIVisible(true);

	Destroy();

	
}

void ABossSequenceTrigger::SetAllUIVisible(bool bVisible)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* Widget : AllWidgets)
	{
		if (Widget)
		{
			Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		}
	}
}

void ABossSequenceTrigger::SetPlayerInputEnabled(bool bEnabled)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	if (bEnabled)
	{
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
		PC->bShowMouseCursor = false;
	}
	else
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
		PC->bShowMouseCursor = false;
	}
}