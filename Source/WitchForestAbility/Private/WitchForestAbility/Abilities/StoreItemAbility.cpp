// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/StoreItemAbility.h"

#include "WitchForestAbility.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

void UStoreItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	APawn* Pawn = GetOwningPawnFromActorInfo(ActorInfo);

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!Inventory || !ItemHandle)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
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
	ItemToStore->Destroy();
	
	uint8 SelectedIndex = Inventory->GetSelectedIndex();
	FGameplayTag ItemTag;
	if (ItemSet->FindItemTagFromClass(PickupClass, ItemTag))
	{
		Inventory->SetItemBySlot(SelectedIndex, ItemTag);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UE_LOGFMT(LogWitchForestAbility, Error, "StoreItemAbility failed. Could not find an ItemTag matching class '{ClassName}'.", PickupClass->GetName());
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
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
		return false;
	}

	APawn* Pawn = GetOwningPawnFromActorInfo(ActorInfo);
	if (!Pawn)
	{
		return false;
	}

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();

	// Check that our currently selected slot is free, and we are holding an item
	if ((!Inventory || Inventory->GetItemBySlot(Inventory->GetSelectedIndex()) != TAG_ItemEmpty) ||
		(!ItemHandle || !ItemHandle->HoldingItem()))
	{
		return false;
	}
	return true;
}
