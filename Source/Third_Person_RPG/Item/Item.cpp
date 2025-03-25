// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Item/Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"

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
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
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
	if (Player && WeaponClass && Player->CanSetWeapon()) 
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Player;

			ATPRWeapon* SpawnedWeapon = World->SpawnActor<ATPRWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			if (SpawnedWeapon)
			{
				Player->SetWeapon(SpawnedWeapon); 
				Destroy(); 
			}
		}
	}
}

