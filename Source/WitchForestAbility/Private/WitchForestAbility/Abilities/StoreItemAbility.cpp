// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/StoreItemAbility.h"

#include "WitchForestAbility.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

void UStoreItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!Inventory || !ItemHandle)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ItemHandle->GetHeldItem() && !ItemHandle->GetHeldItem()->CanBeStored())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameState(Owner)->GameModeClass->ClassDefaultObject);
	if (!GameMode)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. The GameMode is the wrong type.");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. WitchForestGameMode '{ModeName}' does not have a valid ItemSet.", GameMode->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APickup* ItemToStore = ItemHandle->ConsumeItem();
	if (!ItemToStore)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. Failed to consume item from ItemHandle '{HandleName}'.", ItemHandle->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TSubclassOf<APickup> PickupClass = ItemToStore->GetClass();	
	uint8 SelectedIndex = Inventory->GetSelectedIndex();
	FGameplayTag ItemTag;
	bool bFoundItem = ItemSet->FindItemTagFromClass(PickupClass, ItemTag);

	if (!bFoundItem)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. Could not find an ItemTag matching class '{ClassName}'.", PickupClass->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsPredictingClient())
	{
		ItemToStore->SetActorHiddenInGame(true);
	}
	else
	{
		ItemToStore->Destroy();
		Inventory->SetItemBySlot(SelectedIndex, ItemTag);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	return;
}


bool UStoreItemAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AActor* Owner = ActorInfo->OwnerActor.Get();
	if (!Owner)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. Owner was invalid'.");
		return false;
	}

	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		// UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. Owner '{OwnerName}' was not a pawn.", Owner->GetName());
		return false;
	}

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();

	// Check that our currently selected slot is free, and we are holding an item
	if ((!Inventory || Inventory->GetItemBySlot(Inventory->GetSelectedIndex()) != TAG_ItemEmpty) ||
		(!ItemHandle || !ItemHandle->HoldingItem() || !ItemHandle->GetHeldItem()->CanBeStored()))
	{
		return false;
	}

	return true;
}

void UStoreItemAbility::ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey)
{
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		return;
	}

	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		return;
	}

	ItemHandle->SetHeldItemHidden(false);
}

void UStoreItemAbility::ActivateAbilitySucceed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FPredictionKey& PredictionKey)
{
	AActor* Owner = ActorInfo->OwnerActor.Get();
	if (!Owner)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameState(Owner)->GameModeClass->ClassDefaultObject);
	if (!GameMode)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	if (!Inventory || !ItemHandle)
	{
		return;
	}

	if (APickup* ConsumedItem = ItemHandle->ConsumeItem())
	{
		ConsumedItem->Destroy();
		TSubclassOf<APickup> PickupClass = ConsumedItem->GetClass();
		uint8 SelectedIndex = Inventory->GetSelectedIndex();
		FGameplayTag ItemTag;
		if (ItemSet->FindItemTagFromClass(PickupClass, ItemTag))
		{
			Inventory->SetItemBySlot(SelectedIndex, ItemTag);
		}
	}
}
