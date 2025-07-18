// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/GameMode/TPRGameMode.h"

#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/HUD.h"



ATPRGameMode::ATPRGameMode()
{
	//DefaultPawn 설정
	//_C는 해당 블루프린트 클래스의 인스턴스를 구별하기 위한 언리얼의 네이밍 컨벤션
	//_C는 해당 블루프린트 클래스의 인스턴스를 가리킴
	//블루프린트 클래스를 경로로 찾아올 때는 "_C"를 붙여줘야 함
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/BluePrint/BP_PlayerCharacter.BP_PlayerCharacter_C'"));
	if (PawnClassRef.Succeeded())
	{
		DefaultPawnClass = PawnClassRef.Class;
	}

	//PlayerControllerClass 설정
	static ConstructorHelpers::FClassFinder<APlayerController> ControllerClassRef(TEXT("/Script/CoreUObject.Class'/Script/Third_Person_RPG.TPRPlayerController'"));
	if (ControllerClassRef.Succeeded())
	{
		PlayerControllerClass = ControllerClassRef.Class;
	}
}

void ATPRGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		// 뷰포트에 추가된 위젯을 추적하고 있다면 이걸 제거
		if (PC->GetHUD())
		{
			// 방법 1: 로딩 위젯을 GameInstance 또는 PlayerController에서 직접 추적한 뒤 제거
			if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
			{
				if (GI->LoadingScreenWidget)
				{
					GI->LoadingScreenWidget->RemoveFromParent();
					GI->LoadingScreenWidget = nullptr;
				}
			}
		}
	}
}