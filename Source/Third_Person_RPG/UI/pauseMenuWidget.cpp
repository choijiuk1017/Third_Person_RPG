#include "Third_Person_RPG/UI/pauseMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UpauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (Button_ReturnToTitle)
	{
		Button_ReturnToTitle->OnClicked.AddDynamic(this, &UpauseMenuWidget::OnReturnToTitleClicked);
	}

	if (Button_QuitGame)
	{
		Button_QuitGame->OnClicked.AddDynamic(this, &UpauseMenuWidget::OnQuitGameClicked);
	}
}

void UpauseMenuWidget::Open(APlayerController* InPC)
{
	OwningPC = InPC;

	AddToViewport(9999);

	if (OwningPC)
	{
		OwningPC->SetPause(true);
		OwningPC->bShowMouseCursor = true;

		FInputModeUIOnly Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		OwningPC->SetInputMode(Mode);
	}

	// ★ 포커스 줘야 ESC가 위젯으로 들어옵니다
	SetKeyboardFocus();
}

void UpauseMenuWidget::Close()
{
	RemoveFromParent();

	if (OwningPC)
	{
		OwningPC->SetPause(false);
		OwningPC->bShowMouseCursor = false;

		FInputModeGameOnly Mode;
		OwningPC->SetInputMode(Mode);
	}
}

FReply UpauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Escape)
	{
		Close();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UpauseMenuWidget::OnReturnToTitleClicked()
{
	if (OwningPC) OwningPC->SetPause(false);
	UGameplayStatics::OpenLevel(GetWorld(), TitleLevelName);
}

void UpauseMenuWidget::OnQuitGameClicked()
{
	if (!OwningPC)
	{
		OwningPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	UKismetSystemLibrary::QuitGame(GetWorld(), OwningPC, EQuitPreference::Quit, false);
}