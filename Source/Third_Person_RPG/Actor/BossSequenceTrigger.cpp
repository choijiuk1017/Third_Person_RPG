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

#include "Third_Person_RPG/Instance/TPRGameInstance.h"

// Sets default values
ABossSequenceTrigger::ABossSequenceTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossSequenceTrigger::OnTriggerOverlap);

	for (AStaticMeshActor* BlockActor : BossRoomBlock)
	{
		if (IsValid(BlockActor))
		{
			BlockActor->SetActorHiddenInGame(true);
			BlockActor->SetActorEnableCollision(false);
		}
	}

}

// Called when the game starts or when spawned
void ABossSequenceTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (AmbientBGMActor)
	{
		AmbientBGMComp = AmbientBGMActor->FindComponentByClass<UAudioComponent>();
	}
	if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(GetGameInstance()))
	{
		if (GI->HasClearedBoss(BossID))
		{
			bHasClearedBoss = true;
		}
	}

	if (bHasClearedBoss)
	{
		for (AStaticMeshActor* MeshActor : StaticMeshActorsToDestroy)
		{
			if (IsValid(MeshActor))
			{
				MeshActor->Destroy();
			}
		}

		

		for (AStaticMeshActor* BlockActor : BossRoomBlock)
		{
			if (IsValid(BlockActor))
			{
				BlockActor->Destroy();
			}
		}
	}

	for (AStaticMeshActor* BlockActor : BossRoomBlock)
	{
		if (IsValid(BlockActor))
		{
			BlockActor->SetActorHiddenInGame(true);
			BlockActor->SetActorEnableCollision(false);
		}
	}

	for (ASkeletalMeshActor* SkeletalActor : SkeletalMeshActorsToDestroy)
	{
		if (IsValid(SkeletalActor))
		{
			SkeletalActor->SetActorHiddenInGame(true);
			SkeletalActor->SetActorEnableCollision(false);
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
	if (bHasClearedBoss) return;

	for (ASkeletalMeshActor* SkeletalActor : SkeletalMeshActorsToDestroy)
	{
		if (IsValid(SkeletalActor))
		{
			SkeletalActor->SetActorHiddenInGame(false);
			SkeletalActor->SetActorEnableCollision(true);
		}
	}

	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		if (APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
		{
			PC->ForceStopActionsForCutscene();
		}

		bSpawnHiddenBossThisRun = false;

		ALevelSequenceActor* ChosenSequence = SequenceActor;

		if (bIsHiddenBoss && HiddenBossSequenceActor)
		{
			UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GI && GI->bHiddenBossUnlocked)
			{
				bSpawnHiddenBossThisRun = true;
				ChosenSequence = HiddenBossSequenceActor;
			}
			else
			{

				bSpawnHiddenBossThisRun = false;
				ChosenSequence = SequenceActor;
			}
		}

		if (!ChosenSequence) return;

		ULevelSequencePlayer* Player = ChosenSequence->GetSequencePlayer();
		if (!Player) return;

		bHasPlayed = true;

		SetPlayerInputEnabled(false);
		SetAllUIVisible(false);

		PlayedSequenceActor = ChosenSequence;

		Player->OnFinished.AddDynamic(this, &ABossSequenceTrigger::OnSequenceEnd);
		StartBossBGM();
		Player->Play();
	}
}

void ABossSequenceTrigger::OnSequenceEnd()
{
	ALevelSequenceActor* SeqActorForTransform = PlayedSequenceActor ? PlayedSequenceActor : SequenceActor;
	if (!SeqActorForTransform)
	{
		SetPlayerInputEnabled(true);
		SetAllUIVisible(true);
		Destroy();
		return;
	}

	FTransform SpawnTransform = SequenceActor->GetTransform();
	SpawnTransform.AddToTranslation(FVector(0, 0, 100.f));

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

	if (bSpawnHiddenBossThisRun)
	{
		if (HiddenBossClass && SpawnTransform.IsValid())
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(HiddenBossClass, SpawnTransform, Params);
		}
	}
	else
	{
		if (CombatBossClass && SpawnTransform.IsValid())
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(CombatBossClass, SpawnTransform, Params);
		}
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

	PC->FlushPressedKeys();

	if (bEnabled)
	{
		PC->EnableInput(PC);
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		PC->DisableInput(PC);

		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void ABossSequenceTrigger::StartBossBGM()
{
	if (AmbientBGMComp)
	{
		if (FadeOutTime > 0.f)
		{
			AmbientBGMComp->FadeOut(FadeOutTime, 0.f);
		}
		else
		{
			AmbientBGMComp->SetPaused(true);
		}
	}

	if (!BossBGM) return;

	if (!BossBGMComp)
	{
		BossBGMComp = UGameplayStatics::SpawnSound2D(
			GetWorld(), BossBGM, 1.0f, 1.0f, 0.0f, nullptr, false, false
		);
	}

	if (BossBGMComp)
	{
		if (FadeInTime > 0.f) BossBGMComp->FadeIn(FadeInTime, 1.0f);
		else BossBGMComp->Play();
	}
}

void ABossSequenceTrigger::StopBossBGMAndResumeAmbient()
{
	if (BossBGMComp)
	{
		if (FadeOutTime > 0.f) BossBGMComp->FadeOut(FadeOutTime, 0.f);
		else BossBGMComp->Stop();

		BossBGMComp = nullptr;
	}

	if (AmbientBGMComp && bResumeAmbientAfterBoss)
	{
		if (FadeInTime > 0.f) AmbientBGMComp->FadeIn(FadeInTime, 1.0f);
		else AmbientBGMComp->SetPaused(false);
	}
}