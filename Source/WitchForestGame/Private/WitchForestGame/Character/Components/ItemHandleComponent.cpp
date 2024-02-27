// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Components/ItemHandleComponent.h"

#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestAbility/WitchForestASC.h"

UItemHandleComponent::UItemHandleComponent()
{
}

APickup* UItemHandleComponent::ConsumeItem()
{
	if (!HeldItem.IsValid())
	{
		return nullptr;
	}
	APickup* Item = HeldItem.Get();
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->bHeld = false;
	HeldItem.Reset();

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
	if (HeldItem.IsValid())
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
	return HeldItem.IsValid();
}
