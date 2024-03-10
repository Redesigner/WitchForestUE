// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Components/ItemHandleComponent.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


UItemHandleComponent::UItemHandleComponent()
{
	SetIsReplicatedByDefault(true);
}

void UItemHandleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, HeldItem);
}

void UItemHandleComponent::OnRep_HeldItem(APickup* OldHeldItem)
{
	if (HeldItem == OldHeldItem)
	{
		return;
	}

	// We want to update our granted abilities and attributes if these held items are somehow different classes
	if (HeldItem && OldHeldItem && HeldItem->GetClass() != OldHeldItem->GetClass())
	{
		if (AWitch* Witch = Cast<AWitch>(GetOwner()))
		{
			if (AWitchPlayerState* PlayerState = Cast<AWitchPlayerState>(Witch->GetPlayerState()))
			{
				UE_LOGFMT(LogWitchForestGame, Display, "ItemHandleComponent '{SelfName}' in '{ActorName}' Held Item Replication: potential granted ability set mismatch", GetName(), GetOwner()->GetName());
				GrantedHandles.TakeFromAbilitySystem(PlayerState->GetWitchForestASC());
				
				if (HeldItem->GetGrantedAbilitySet())
				{
					HeldItem->GetGrantedAbilitySet()->GiveToAbilitySystem(PlayerState->GetWitchForestASC(), &GrantedHandles);
				}
			}
		}
	}

	// Check if we need to drop our old item -- either we're carrying the wrong item,
	// or we've just replicated that we aren't actually holding an item
	if (OldHeldItem)
	{
		// if ((HeldItem.IsValid() && HeldItem != OldHeldItem) || !HeldItem.IsValid())
		{
			UE_LOGFMT(LogWitchForestGame, Display, "ItemHandleComponent '{SelfName}' in '{ActorName}' Held Item Replication: previously held item '{ItemName}' was incorrect, dropping item.", GetName(), GetOwner()->GetName(), OldHeldItem->GetName());

			OldHeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			OldHeldItem->bHeld = false;
			OldHeldItem->EnableMovement();
		}
	}

	// Check if we need to pick up a new item -- we have found out through replication
	// that we are carrying the wrong item, or we thought we were carrying nothing
	// but are actually carrying a new item
	if (HeldItem)
	{
		if ((OldHeldItem && OldHeldItem != HeldItem) || !OldHeldItem)
		{
			UE_LOGFMT(LogWitchForestGame, Display, "ItemHandleComponent '{SelfName}' in '{ActorName}' Held Item Replication: previously held item '{ItemName}' was incorrect, picking up item '{NewItemname}'.",
				GetName(), GetOwner()->GetName(), OldHeldItem ? OldHeldItem->GetName() : "None", HeldItem->GetName());

			HeldItem->DisableMovement();
			HeldItem->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			HeldItem->bHeld = true;
		}
	}
}

void UItemHandleComponent::DropAllItems()
{
	TArray<AActor*> AttachedActors;
	GetOwner()->GetAttachedActors(AttachedActors);
	UE_LOGFMT(LogWitchForestGame, Display, "ItemHandleComponent '{SelfName}' in '{ActorName}' was holding {ItemCount} items.", GetName(), GetOwner()->GetName(), AttachedActors.Num());
	for (AActor* AttachedActor : AttachedActors)
	{
		if (APickup* AttachedPickup = Cast<APickup>(AttachedActor))
		{
			AttachedPickup->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			AttachedPickup->bHeld = false;
			AttachedPickup->EnableMovement();
		}
	}
}

void UItemHandleComponent::SetFakePickup(APickup* Fake)
{
	if (FakePickup.IsValid())
	{
		FakePickup->Destroy();
	}

	FakePickup = Fake;
}

APickup* UItemHandleComponent::ConsumeItem()
{
	if (!HeldItem)
	{
		return nullptr;
	}

	APickup* Item = HeldItem.Get();
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->bHeld = false;
	HeldItem = nullptr;

	if (AWitch* Witch = Cast<AWitch>(GetOwner()))
	{
		if (AWitchPlayerState* PlayerState = Cast<AWitchPlayerState>(Witch->GetPlayerState()))
		{
			GrantedHandles.TakeFromAbilitySystem(PlayerState->GetWitchForestASC());
		}
	}

	Item->EnableMovement();
	return Item;
}

void UItemHandleComponent::PickupItem(APickup* Item)
{
	if (HeldItem)
	{
		return;
	}

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return;
	}

	HeldItem = Item;
	Item->DisableMovement();
	Item->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Item->bHeld = true;
	if (AWitchPlayerState* PlayerState = Cast<AWitchPlayerState>(PawnOwner->GetPlayerState()))
	{
		if (!Item->GetGrantedAbilitySet())
		{
			return;
		}
		Item->GetGrantedAbilitySet()->GiveToAbilitySystem(PlayerState->GetWitchForestASC(), &GrantedHandles);
	}
}

bool UItemHandleComponent::HoldingItem() const
{
	return HeldItem != nullptr;
}

void UItemHandleComponent::SetHeldItemHidden(bool bHidden)
{
	if (HeldItem)
	{
		HeldItem->SetActorHiddenInGame(bHidden);
	}
}
