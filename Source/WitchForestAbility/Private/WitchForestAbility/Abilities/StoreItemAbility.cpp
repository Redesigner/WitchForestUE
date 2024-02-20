// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/StoreItemAbility.h"

#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void UStoreItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Pawn = Owner;

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	if (!Inventory || Inventory->GetItemBySlot(Inventory->GetSelectedIndex()) != TAG_ItemEmpty) // If the current slot is not empty, exit
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	// Since the ASC can live on the PlayerState for player characters, check here and get their pawns.
	// Otherwise, the ASC should be owned by the pawn -- for enemies and other NPCs
	if (APlayerState* PlayerState = Cast<APlayerState>(Owner))
	{
		Pawn = PlayerState->GetPawn();
	}
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle || !ItemHandle->HoldingItem())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	APickup* ItemToStore = ItemHandle->ConsumeItem();
	if (!ItemToStore)
	{
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
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	return;
}