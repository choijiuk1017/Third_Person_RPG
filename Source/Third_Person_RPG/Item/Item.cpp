// Fill out your copyright notice in the Description page of Project Settings.


#include "Third_Person_RPG/Item/Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "Third_Person_RPG/UI/InteractionWidget.h"
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
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);


	static ConstructorHelpers::FClassFinder<UInteractionWidget> InteractionWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BluePrint/WBP_Interaction.WBP_Interaction_C'"));
	if (InteractionWidgetRef.Class)
	{
		InteractionWidgetClass = InteractionWidgetRef.Class;
	}

	HelpText = TEXT("Press 'E' to pick up the item.");
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
	if (InteractionWidget)
	{
		InteractionWidget->SetHelpText(HelpText);
		InteractionWidget->AddToViewport();
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
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
	}

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{
		Player->ReSetOverlappingItem(); // 새로 만든 함수
	}

	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

