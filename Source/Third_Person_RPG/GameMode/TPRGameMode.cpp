// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/GameMode/TPRGameMode.h"

ATPRGameMode::ATPRGameMode()
{
	//DefaultPawn ����
	//_C�� �ش� �������Ʈ Ŭ������ �ν��Ͻ��� �����ϱ� ���� �𸮾��� ���̹� ������
	//_C�� �ش� �������Ʈ Ŭ������ �ν��Ͻ��� ����Ŵ
	//�������Ʈ Ŭ������ ��η� ã�ƿ� ���� "_C"�� �ٿ���� ��
	static ConstructorHelpers::FClassFinder<APawn> PawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/BluePrint/BP_PlayerCharacter.BP_PlayerCharacter_C'"));
	if (PawnClassRef.Succeeded())
	{
		DefaultPawnClass = PawnClassRef.Class;
	}

	//PlayerControllerClass ����
	static ConstructorHelpers::FClassFinder<APlayerController> ControllerClassRef(TEXT("/Script/CoreUObject.Class'/Script/Third_Person_RPG.TPRPlayerController'"));
	if (ControllerClassRef.Succeeded())
	{
		PlayerControllerClass = ControllerClassRef.Class;
	}
}