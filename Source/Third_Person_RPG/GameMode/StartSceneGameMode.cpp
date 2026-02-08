// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/GameMode/StartSceneGameMode.h"

AStartSceneGameMode::AStartSceneGameMode()
{
	DefaultPawnClass = nullptr;

	static ConstructorHelpers::FClassFinder<APlayerController> ControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/BluePrint/Player_Controller.Player_Controller'"));
	if (ControllerClassRef.Succeeded())
	{
		PlayerControllerClass = ControllerClassRef.Class;
	}
}