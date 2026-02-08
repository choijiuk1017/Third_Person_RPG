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
	Trigger->SetupAttachment(GetCapsuleComponent());  
	Trigger->SetRelativeLocation(FVector::ZeroVector); 
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

	if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(GetGameInstance()))
	{
		if (GI->HasMetNPC(NPCID))
		{
			bHasMetPlayer = true;
		}
	}
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
	if (bIsTalking) return;

	Player->HideInteractionUI();

	if (!bHasMetPlayer && FirstEncounterLines.Num() > 0)
	{
		SetTalkPhase(ENPCTalkPhase::FirstEncounter);
	}
	else
	{
		SetTalkPhase(ENPCTalkPhase::Repeat);
	}


	if (!ActiveLines || ActiveLines->Num() == 0)
	{
		return;
	}

	bIsTalking = true;

	Player->OpenDialogueUI();
	Player->SetDialogueLine((*ActiveLines)[CurrentLineIndex]);
}

void ANPC::AdvanceTalk(APlayerCharacter* Player)
{
	if (!Player) return;
	if (!bIsTalking || !ActiveLines) return;

	Player->HideInteractionUI();

	if (bEndAfterChoice)
	{
		bEndAfterChoice = false;
		EndTalk(Player);
		return;
	}


	CurrentLineIndex++;

	if (CurrentLineIndex < ActiveLines->Num())
	{
		Player->SetDialogueLine((*ActiveLines)[CurrentLineIndex]);
		return;
	}

	if (TalkPhase == ENPCTalkPhase::FirstEncounter)
	{
		bHasMetPlayer = true;

		if (UTPRGameInstance* GI = Cast<UTPRGameInstance>(GetGameInstance()))
		{
			GI->RegisterMetNPC(NPCID);
		}

		if (RepeatLines.Num() > 0)
		{
			SetTalkPhase(ENPCTalkPhase::Repeat);
			if (ActiveLines && ActiveLines->Num() > 0)
			{
				Player->SetDialogueLine((*ActiveLines)[0]);
			}
			return;
		}
	}

	if (bHasChoice)
	{
		BeginChoice(Player);
		return;
	}

	EndTalk(Player);
}

void ANPC::BeginChoice(APlayerCharacter* Player)
{
	if (!Player) return;

	InteractionPhase = ENPCInteractionPhase::Choosing;

	Player->OpenChoiceUI(ChoiceQuestion, YesText, NoText);
}

void ANPC::ConfirmChoice(APlayerCharacter* Player, bool bYes)
{
	if (!Player) return;
	if (InteractionPhase != ENPCInteractionPhase::Choosing) return;

	Player->CloseChoiceUI();

	InteractionPhase = ENPCInteractionPhase::Ended;

	if (bYes) OnYesSelected(Player);
	else      OnNoSelected(Player);

	Player->OpenDialogueUI();

	bEndAfterChoice = true;
}


void ANPC::EndTalk(APlayerCharacter* Player)
{
	if (!Player) return;

	Player->HideInteractionUI();

	bIsTalking = false;
	CurrentLineIndex = 0;
	ActiveLines = nullptr;
	TalkPhase = ENPCTalkPhase::None;
	InteractionPhase = ENPCInteractionPhase::Ended;

	Player->CloseDialogueUI();
	Player->CloseChoiceUI();

	OnTalkFinished(Player);
}

void ANPC::SetTalkPhase(ENPCTalkPhase NewPhase)
{
	TalkPhase = NewPhase;
	CurrentLineIndex = 0;

	switch (TalkPhase)
	{
	case ENPCTalkPhase::FirstEncounter:
		ActiveLines = &FirstEncounterLines;
		break;
	case ENPCTalkPhase::Repeat:
		ActiveLines = &RepeatLines;
		break;
	default:
		ActiveLines = nullptr;
		break;
	}
}

void ANPC::OnTalkFinished(APlayerCharacter* Player)
{
}

void ANPC::OnYesSelected(APlayerCharacter* Player)
{
}

void ANPC::OnNoSelected(APlayerCharacter* Player)
{
}

