// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/WithdrawItemAbility.h"

#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void UWithdrawItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Pawn = Owner;

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
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
	if (!ItemHandle || ItemHandle->HoldingItem())
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
	UE_LOG(LogTemp, Display, TEXT("Withdrawing item %s"), *HeldItemTag.GetTagName().ToString());

	// Get the data table for items from our game mode. Should this be different?
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	FInventoryItemData ItemData;
	if (GameMode->GetItemSet()->FindItemDataForTag(HeldItemTag, ItemData))
	{
		APickup* PickedUpItem = GetWorld()->SpawnActorDeferred<APickup>(ItemData.PickupClass, Pawn->GetTransform());
		PickedUpItem->FinishSpawning(Pawn->GetTransform());
		ItemHandle->PickupItem(PickedUpItem);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
