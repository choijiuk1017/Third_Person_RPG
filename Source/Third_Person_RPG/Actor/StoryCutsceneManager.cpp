// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/StoryCutsceneManager.h"


#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/UI/Start/StoryCutsceneWidget.h"
#include "GameFramework/PlayerController.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Engine/Texture2D.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


AStoryCutsceneManager::AStoryCutsceneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AStoryCutsceneManager::BeginPlay()
{
	Super::BeginPlay();
}

void AStoryCutsceneManager::EnsureWidget()
{
	if (WidgetInstance) return;
	if (!WidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	WidgetInstance = CreateWidget<UStoryCutsceneWidget>(PC, WidgetClass);
	if (WidgetInstance)
	{
		WidgetInstance->AddToViewport(ZOrder);
	}
}

void AStoryCutsceneManager::ShowWidgetOnly()
{
	EnsureWidget();
}

void AStoryCutsceneManager::HideWidget()
{
	if (!WidgetInstance) return;

	WidgetInstance->RemoveFromParent();
	WidgetInstance = nullptr;
}

void AStoryCutsceneManager::ShowSlide(int32 Index)
{
	if (bAutoCreateOnShow)
	{
		EnsureWidget();
	}

	if (!WidgetInstance) return;

	if (!IsValidIndex(Index)) return;

	UTexture2D* Tex = SlideImages[Index];
	const FText& Txt = SlideTexts[Index];

	WidgetInstance->SetSlide(Tex, Txt);
}

bool AStoryCutsceneManager::IsValidIndex(int32 Index) const
{
	return SlideImages.IsValidIndex(Index) && SlideTexts.IsValidIndex(Index);
}

void AStoryCutsceneManager::DestroySkeletalMesh()
{
	for (ASkeletalMeshActor* SkeletalActor : SkeletalMeshActorsToDestroy)
	{
		if (IsValid(SkeletalActor))
		{
			SkeletalActor->Destroy();
		}
	}

	for (AStaticMeshActor* MeshActor : StaticMeshActorsToDestroy)
	{
		if (IsValid(MeshActor))
		{
			MeshActor->Destroy();
		}
	}

	SetPlayerInputEnabled(true);
	SetAllUIVisible(true);
}

void AStoryCutsceneManager::SetAllUIVisible(bool bVisible)
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

void AStoryCutsceneManager::SetPlayerInputEnabled(bool bEnabled)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

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

void AStoryCutsceneManager::OpenTutorial()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn());
	if (!Player)
	{
		return ;
	}

	Player->OpenMovementTutorial();
}