// Fill out your copyright notice in the Description page of Project Settings.

#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Character/EnemyCharacter.h"
#include "Third_Person_RPG/Character/BossCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Components/PrimitiveComponent.h"

#define CHANNEL_ACTION ECollisionChannel::ECC_GameTraceChannel2

// Sets default values
ATPRWeapon::ATPRWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(Weapon);  
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitBox->SetCollisionObjectType(ECC_WorldDynamic); 
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	HitBox->SetBoxExtent(FVector(10.0f, 20.0f, 50.0f));
	HitBox->SetGenerateOverlapEvents(true);

	if (SkeletalMeshAsset)
	{
		Weapon->SetSkeletalMesh(SkeletalMeshAsset);
	}

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}


// Called when the game starts or when spawned
void ATPRWeapon::BeginPlay()
{
	Super::BeginPlay();
	

	if (HitBox)
	{
		HitBox->OnComponentBeginOverlap.AddDynamic(this, &ATPRWeapon::OnHitBoxOverlap);
	}


}

// Called every frame
void ATPRWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HitBox)
	{
		FVector Location = HitBox->GetComponentLocation();
		FVector Extent = HitBox->GetScaledBoxExtent();
		FRotator Rotation = HitBox->GetComponentRotation();

		// ��ġ�� ���Ͱ� �ִ��� �˻� (����ȭ�� ���)
		bool bHasOverlap = false;

		TArray<AActor*> OverlappingActors;
		HitBox->GetOverlappingActors(OverlappingActors);

		for (AActor* Actor : OverlappingActors)
		{
			if (Actor && Actor != GetOwner())
			{
				UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
				if (Comp && Comp->GetCollisionObjectType() == ECC_GameTraceChannel1)
				{
					bHasOverlap = true;
					break;
				}
			}
		}

		FColor BoxColor = bHasOverlap ? FColor::Green : FColor::Red;


		DrawDebugBox(
			GetWorld(),
			Location,
			Extent,
			Rotation.Quaternion(),
			BoxColor,
			false,
			-1.0f,
			0,
			1.0f
		);


	}

}

void ATPRWeapon::EnableHitBox()
{
	DamagedActors.Empty();

	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ATPRWeapon::DisableHitBox()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATPRWeapon::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor && OtherActor != GetOwner())
	{
		if (DamagedActors.Contains(OtherActor))
		{
			return; 
		}

		if (OtherActor->GetClass()->ImplementsInterface(UAnimationAttackInterface::StaticClass()))
		{
			if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("Enemy Damaged via Weapon HitBox"));

				Enemy->RegisterAttacker(GetOwner());

				APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner());

				Enemy->TakeDamage(PlayerOwner->CombatStats.AttackPower);

				DamagedActors.Add(OtherActor);
			}

			if (ABossCharacter* Boss = Cast<ABossCharacter>(OtherActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("Boss Damaged via Weapon HitBox"));

				Boss->RegisterAttacker(GetOwner());

				APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner());

				Boss->TakeDamage(PlayerOwner->CombatStats.AttackPower);

				DamagedActors.Add(OtherActor);
			}
		}
	}
}

