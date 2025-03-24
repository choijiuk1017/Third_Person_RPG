// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Weapon/TPRWeapon.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Third_Person_RPG/Character/PlayerCharacter.h"

// Sets default values
ATPRWeapon::ATPRWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));
	WeaponCollision->SetupAttachment(WeaponMesh);
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WeaponCollision->SetSphereRadius(50.f);

	// Overlap 이벤트 바인딩
	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &ATPRWeapon::OnWeaponOverlap);

	WeaponType = WeaponType::Sword;
	BaseSocketName = FName("WeaponSocket");

}

void ATPRWeapon::EquipWeapon(ACharacter* Player)
{
	if (!Player || !WeaponMesh)
		return;

	// 플레이어의 Skeletal Mesh 가져오기
	USkeletalMeshComponent* PlayerMesh = Player->GetMesh();
	if (!PlayerMesh)
		return;

	// 기본 소켓 이름 설정 (혹은 외부에서 설정 가능)
	BaseSocketName = FName("WeaponSocket"); // 손 소켓 이름

	// 무기 액터를 플레이어에 Attach
	FAttachmentTransformRules AttachRules(EAttachmentRule::KeepRelative, true);
	AttachToComponent(PlayerMesh, AttachRules, BaseSocketName);
}

void ATPRWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("무기 충돌 감지됨!"));
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어와 충돌! 장착 시도"));
		EquipWeapon(Player);

		// 충돌 제거 (중복 장착 방지)
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
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

