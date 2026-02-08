// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/StoryDialogueManager.h"

#include "Kismet/GameplayStatics.h"
#include "Third_Person_RPG/UI/Start/StoryDialogueWidget.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AStoryDialogueManager::AStoryDialogueManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStoryDialogueManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStoryDialogueManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStoryDialogueManager::EnsureWidget()
{
	if (WidgetInstance) return;
	if (!WidgetClass) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	WidgetInstance = CreateWidget<UStoryDialogueWidget>(PC, WidgetClass);
	if (WidgetInstance)
	{
		WidgetInstance->AddToViewport(ZOrder);
	}
}

void AStoryDialogueManager::ShowWidgetOnly()
{
	EnsureWidget();
}

void AStoryDialogueManager::HideDialogue()
{
	if (!WidgetInstance) return;

	WidgetInstance->RemoveFromParent();
	WidgetInstance = nullptr;
}

void AStoryDialogueManager::ShowDialogue(int32 Index)
{
	if (bAutoCreateOnShow)
	{
		EnsureWidget();
	}

	if (!WidgetInstance) return;

	if (!IsValidIndex(Index)) return;

	const FText& Txt = SlideTexts[Index];

	WidgetInstance->SetSlide(Txt);
}

bool AStoryDialogueManager::IsValidIndex(int32 Index) const
{
	return SlideTexts.IsValidIndex(Index);
}


