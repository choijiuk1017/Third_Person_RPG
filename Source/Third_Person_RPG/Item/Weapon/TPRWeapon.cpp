// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"

// Sets default values
ATPRWeapon::ATPRWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;

	// 에디터에서 설정한 SkeletalMeshAsset을 여기서 사용
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
	
}

// Called every frame
void ATPRWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

