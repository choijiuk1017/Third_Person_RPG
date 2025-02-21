// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/GameMode/TPRGameMode.h"

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