// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Character/LastBoss.h"
#include "Components/StaticMeshComponent.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


ALastBoss::ALastBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));


	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));


	//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ALastBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead && !bCutsceneStarted)
	{
		bCutsceneStarted = true;

		GetWorldTimerManager().SetTimer(
			CutSceneStartTimer,
			this,
			&ALastBoss::StartCutScene,
			3.0f,
			false
		);
	}
}

void ALastBoss::StartCutScene()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		Player->SetActorHiddenInGame(true);
		Player->SetActorEnableCollision(false);
	}

	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		GetWorld(),
		Widgets,
		UUserWidget::StaticClass(),
		false
	);

	for (UUserWidget* W : Widgets)
	{
		W->RemoveFromParent();
	}

	if (!BossCutscene)
	{
		UE_LOG(LogTemp, Error, TEXT("BossCutscene asset not assigned!"));
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleMap"));
		return;
	}
	FMovieSceneSequencePlaybackSettings Settings;
	ALevelSequenceActor* OutActor;

	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		BossCutscene,
		Settings,
		OutActor
	);

	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.AddDynamic(this, &ALastBoss::OnCutSceneFinished);
		SequencePlayer->Play();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create SequencePlayer"));
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleMap"));
	}
}

void ALastBoss::OnCutSceneFinished()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("TitleMap"));
}
