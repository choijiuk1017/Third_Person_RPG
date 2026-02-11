// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Item/Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "Third_Person_RPG/UI/InteractionWidget.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->InitSphereRadius(100.f);
	RootComponent = Trigger;
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);

	BoxMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BoxMesh"));
	BoxMesh->SetupAttachment(RootComponent);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);

	HelpText = FText::FromString(TEXT("'F'키를 눌러 획득"));
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (ItemData)
	{
		FPrimaryAssetId AssetId = ItemData->GetPrimaryAssetId();

		if (const UTPRGameInstance* GI = Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
		{
			if (GI->HasCollectedItemAsset(AssetId))
			{
				Destroy();
				return;
			}
		}
	}

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		Player->SetOverlappingItem(this); // 새로 만든 함수
		Player->ShowInteractionUI(HelpText);
	}
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		Player->ReSetOverlappingItem(); // 새로 만든 함수
		Player->HideInteractionUI();
	}
}

