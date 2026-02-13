// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/UI/TitleScreenWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Third_Person_RPG/TPRSaveGame.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"


void UTitleScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;

	UpdateSelection();

	bIsFocusable = true;

	SetKeyboardFocus();

	if (StartAnim)
	{
		PlayAnimation(StartAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UTitleScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UTitleScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Down)
	{
		CurrentIndex = (CurrentIndex + 1) % 3;
		UpdateSelection();
		return FReply::Handled();
	}

	if (Key == EKeys::Up)
	{
		CurrentIndex = (CurrentIndex - 1 + 3) % 3;
		UpdateSelection();
		return FReply::Handled();
	}

	if (Key == EKeys::Enter || Key == EKeys::SpaceBar)
	{
		ExecuteSelection();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UTitleScreenWidget::UpdateSelection()
{
	FSlateColor NormalColor = FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f));

	FSlateColor SelectedColor = FSlateColor(FLinearColor(1.0f, 0.8f, 0.3f));

	NewGameText->SetColorAndOpacity(NormalColor);
	LoadGameText->SetColorAndOpacity(NormalColor);
	ExitText->SetColorAndOpacity(NormalColor);

	switch (CurrentIndex)
	{
	case 0: NewGameText->SetColorAndOpacity(SelectedColor); break;
	case 1: LoadGameText->SetColorAndOpacity(SelectedColor); break;
	case 2: ExitText->SetColorAndOpacity(SelectedColor); break;
	}
}

void UTitleScreenWidget::ExecuteSelection()
{
	switch (CurrentIndex)
	{
	case 0:
	{
		if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(GetGameInstance()))
		{
			GI->DeleteSaveData();
		}

		if (LoadingScreen)
		{
			LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
			if (LoadingScreenWidget)
			{
				LoadingScreenWidget->AddToViewport(9999);
			}
		}


		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([=, this]()
		{
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("Start"));
			//UGameplayStatics::OpenLevel(GetWorld(), TEXT("Dungeon1"));
		});

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);

		break;
	}
	case 1:
	{
		const FString SaveSlotName = TEXT("PlayerSaveSlot");
		const int32 UserIndex = 0;

		if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
		{
			if (LoadingScreen)
			{
				LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
				if (LoadingScreenWidget)
				{
					LoadingScreenWidget->AddToViewport(9999);
				}
			}


			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([=, this]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("Start"));
				});

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
			break;
		}

		UTPRSaveGame* Loaded = Cast<UTPRSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
		if (!Loaded)
		{
			if (LoadingScreen)
			{
				LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
				if (LoadingScreenWidget)
				{
					LoadingScreenWidget->AddToViewport(9999);
				}
			}


			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([=, this]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("Start"));
				});

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
			break;
		}

		if (!Loaded->LastSavedMapName.IsNone())
		{
			if (LoadingScreen)
			{
				LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
				if (LoadingScreenWidget)
				{
					LoadingScreenWidget->AddToViewport(9999);
				}
			}


			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([=, this]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), Loaded->LastSavedMapName);
				});

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
			
		}
		else
		{
			if (LoadingScreen)
			{
				LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreen);
				if (LoadingScreenWidget)
				{
					LoadingScreenWidget->AddToViewport(9999);
				}
			}


			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([=, this]()
				{
					UGameplayStatics::OpenLevel(GetWorld(), TEXT("Start"));
				});

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
		}
		break;
	}
	case 2:
		UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
		break;
	}
}