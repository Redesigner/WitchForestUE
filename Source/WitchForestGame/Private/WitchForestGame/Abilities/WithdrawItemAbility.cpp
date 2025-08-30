// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/WithdrawItemAbility.h"

#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void UWithdrawItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Pawn = Cast<APawn>(ActorInfo->AvatarActor);

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!Inventory || !ItemHandle)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// @TODO: This should be packed into the ability activation request by the client, since the server won't know which item is selected
	uint8 SelectedIndex = Inventory->GetSelectedIndex();
	FGameplayTag HeldItemTag = Inventory->ConsumeItem(SelectedIndex);
	if (!HeldItemTag.IsValid() || HeldItemTag.MatchesTagExact(FGameplayTag::EmptyTag))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	// Get the data table for items from our game mode. Should this be different?
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameState(Owner)->GameModeClass->ClassDefaultObject);
	if (!GameMode)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!IsPredictingClient())
	{
		FInventoryItemData ItemData;
		if (GameMode->GetItemSet()->FindItemDataForTag(HeldItemTag, ItemData))
		{
			APickup* PickedUpItem = GetWorld()->SpawnActorDeferred<APickup>(ItemData.PickupClass, Pawn->GetTransform());
			// Set held early, to make sure it isn't collected by any volumes
			PickedUpItem->bHeld = true;
			PickedUpItem->FinishSpawning(Pawn->GetTransform());
			ItemHandle->PickupItem(PickedUpItem);
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}


bool UWithdrawItemAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AActor* Owner = ActorInfo->OwnerActor.Get();
	if (!Owner)
	{
		return false;
	}

	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		return false;
	}

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();

	// Check that our currently selected slot has an item, and we are not holding an item
	if ((!Inventory || Inventory->GetItemBySlot(Inventory->GetSelectedIndex()) == TAG_ItemEmpty) ||
		(!ItemHandle || ItemHandle->HoldingItem()))
	{
		return false;
	}

	return true;
}
