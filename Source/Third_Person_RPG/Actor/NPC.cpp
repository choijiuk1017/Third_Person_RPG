// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Actor/NPC.h"

#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(GetCapsuleComponent());   // ¡Ú ÇÙ½É
	Trigger->SetRelativeLocation(FVector::ZeroVector); // ¡Ú Ä¸½¶ Áß½É¿¡ °íÁ¤
	Trigger->InitSphereRadius(200.f);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));


	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));


	ShieldMesh->SetupAttachment(GetMesh(), TEXT("ShieldSocket"));


	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnOverlapEnd);


	HelpText = FText::FromString(TEXT("Press 'F' to talk"));

}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANPC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		Player->SetCurrentNPC(this);
		Player->ShowInteractionUI(HelpText);
	}
}

void ANPC::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		if (bIsTalking)
		{
			EndTalk(Player);
		}

		Player->SetCurrentNPC(nullptr);

		Player->HideInteractionUI();
	}
}

void ANPC::StartTalk(APlayerCharacter* Player)
{
	if (!Player) return;

	Player->HideInteractionUI();

	if (bIsTalking) return;

	if (!bHasMetPlayer && FirstEncounterLines.Num() > 0)
	{
		ActiveLines = &FirstEncounterLines;
	}
	else
	{
		ActiveLines = &RepeatLines;
	}

	if (!ActiveLines || ActiveLines->Num() == 0) return;

	bIsTalking = true;
	CurrentLineIndex = 0;

	Player->OpenDialogueUI();
	Player->SetDialogueLine((*ActiveLines)[CurrentLineIndex]);

	if (!bHasMetPlayer)
	{
		bHasMetPlayer = true;
	}
}

void ANPC::AdvanceTalk(APlayerCharacter* Player)
{
	if (!Player) return;

	Player->HideInteractionUI();

	if (!bIsTalking || !ActiveLines) return;

	CurrentLineIndex++;

	if (CurrentLineIndex < ActiveLines->Num())
	{
		Player->SetDialogueLine((*ActiveLines)[CurrentLineIndex]);
		return;
	}

	EndTalk(Player);
}

void ANPC::EndTalk(APlayerCharacter* Player)
{
	if (!Player) return;

	Player->HideInteractionUI();

	bIsTalking = false;
	CurrentLineIndex = 0;
	ActiveLines = nullptr;

	Player->CloseDialogueUI();
}



