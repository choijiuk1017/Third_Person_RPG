// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/GameMode/TitleGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!TitleWidgetClass) return;

	TitleWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass);
	if (!TitleWidgetInstance) return;

	TitleWidgetInstance->AddToViewport();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TitleWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;	
	}
}