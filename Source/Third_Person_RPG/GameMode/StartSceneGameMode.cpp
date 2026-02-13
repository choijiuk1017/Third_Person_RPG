// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/GameMode/StartSceneGameMode.h"

AStartSceneGameMode::AStartSceneGameMode()
{
	DefaultPawnClass = nullptr;

    static ConstructorHelpers::FClassFinder<APlayerController> ControllerClassRef(
        TEXT("/Game/BluePrint/Player_Controller.Player_Controller_C")
    );
    if (ControllerClassRef.Succeeded())
    {
        PlayerControllerClass = ControllerClassRef.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Player_Controller class"));
    }
}