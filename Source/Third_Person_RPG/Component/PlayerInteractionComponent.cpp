// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerInteractionComponent.h"

#include "Third_Person_RPG/Character/PlayerCharacter.h"
#include "Third_Person_RPG/Inventory/InventoryComponent.h"
#include "Third_Person_RPG/Item/Item.h"
#include "Third_Person_RPG/Item/Weapon/TPRWeapon.h"
#include "Third_Person_RPG/Actor/SavePoint.h"
#include "Third_Person_RPG/Actor/NPC.h"
#include "Third_Person_RPG/Instance/TPRGameInstance.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerPlayer)
	{
		OwnerPlayer = Cast<APlayerCharacter>(GetOwner());
	}
}

void UPlayerInteractionComponent::InitializeInteractionComponent(APlayerCharacter* InOwner)
{
	OwnerPlayer = InOwner;
}

void UPlayerInteractionComponent::SetOverlappingItem(AItem* Item)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->OverlappingItem = Item;
}

void UPlayerInteractionComponent::ReSetOverlappingItem()
{
	if (!OwnerPlayer) return;

	OwnerPlayer->OverlappingItem = nullptr;
}

void UPlayerInteractionComponent::SetCurrentNPC(ANPC* InNPC)
{
	if (!OwnerPlayer) return;

	OwnerPlayer->CurrentNPC = InNPC;
}

void UPlayerInteractionComponent::Interact()
{
	if (!OwnerPlayer) return;

	if (OwnerPlayer->bIsRoll ||
		OwnerPlayer->bIsSkillActing ||
		OwnerPlayer->bIsAttacking ||
		OwnerPlayer->bIsInteracting)
	{
		return;
	}

	if (OwnerPlayer->bIsDead) return;

	UE_LOG(LogTemp, Warning, TEXT("상호작용 키 입력됨"));

	UInventoryComponent* Inventory = OwnerPlayer->FindComponentByClass<UInventoryComponent>();

	if (OwnerPlayer->OverlappingItem && OwnerPlayer->OverlappingItem->ItemData)
	{
		const FPrimaryAssetId AssetId =
			OwnerPlayer->OverlappingItem->ItemData->GetPrimaryAssetId();

		if (UTPRGameInstance* GI =
			Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(OwnerPlayer->GetWorld())))
		{
			GI->RegisterCollectedItemAsset(AssetId);
		}

		if (OwnerPlayer->OverlappingItem->WeaponClass && OwnerPlayer->CanSetWeapon())
		{
			OwnerPlayer->bIsInteracting = true;

			OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

			UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();

			if (AnimInstance && OwnerPlayer->PickUpMontage)
			{
				AnimInstance->Montage_Play(OwnerPlayer->PickUpMontage, 1.7f);

				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &UPlayerInteractionComponent::OnEquipAnimationEnd);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, OwnerPlayer->PickUpMontage);

				return;
			}
		}
		else
		{
			if (Inventory)
			{
				int32 OutQuantity = 1;
				Inventory->AddItemByData(OwnerPlayer->OverlappingItem->ItemData, OutQuantity);
			}

			OwnerPlayer->OverlappingItem->Destroy();
			OwnerPlayer->OverlappingItem = nullptr;
			OwnerPlayer->bIsInteracting = false;
		}
	}

	if (OwnerPlayer->OverlappingSavePoint)
	{
		OwnerPlayer->bIsInteracting = true;

		if (OwnerPlayer->CurrentWeapon)
		{
			OwnerPlayer->CurrentWeapon->SetActorHiddenInGame(true);
			OwnerPlayer->CurrentWeapon->SetActorEnableCollision(false);
		}

		if (OwnerPlayer->InteractionWidgetInstance)
		{
			OwnerPlayer->InteractionWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}

		OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		OwnerPlayer->SetStatusHUDVisible(false);

		UAnimInstance* AnimInstance = OwnerPlayer->GetMesh()->GetAnimInstance();

		if (AnimInstance && OwnerPlayer->KneelingDownMontage)
		{
			AnimInstance->Montage_Play(OwnerPlayer->KneelingDownMontage, 1.0f);

			OwnerPlayer->bIsKneeling = true;

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(OwnerPlayer, &APlayerCharacter::InteractingSavePoint);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, OwnerPlayer->KneelingDownMontage);

			return;
		}

		if (UTPRGameInstance* GI =
			Cast<UTPRGameInstance>(UGameplayStatics::GetGameInstance(OwnerPlayer->GetWorld())))
		{
			GI->SaveGameData();
		}
	}

	if (OwnerPlayer->CurrentNPC)
	{
		OwnerPlayer->CurrentNPC->StartTalk(OwnerPlayer);
		return;
	}
}

void UPlayerInteractionComponent::OnEquipAnimationEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerPlayer) return;

	UE_LOG(LogTemp, Warning, TEXT("장비 획득"));

	if (!OwnerPlayer->OverlappingItem || !OwnerPlayer->OverlappingItem->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappingItem이 nullptr 또는 ItemData 없음"));
		return;
	}

	UInventoryComponent* Inventory = OwnerPlayer->FindComponentByClass<UInventoryComponent>();

	if (Inventory)
	{
		int32 OutQuantity = 1;
		Inventory->AddItemByData(OwnerPlayer->OverlappingItem->ItemData, OutQuantity);
	}

	OwnerPlayer->OverlappingItem->Destroy();
	OwnerPlayer->OverlappingItem = nullptr;

	OwnerPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerPlayer->bIsInteracting = false;
}
