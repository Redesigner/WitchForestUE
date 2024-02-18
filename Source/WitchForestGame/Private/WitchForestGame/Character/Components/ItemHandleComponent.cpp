// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Components/ItemHandleComponent.h"

#include "WitchForestGame/Dynamic/Pickup.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
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
	HeldItem.Reset();

	if (AWitchPlayerState* PlayerState = Cast<AWitchPlayerState>(GetOwner()))
	{
		GrantedHandles.TakeFromAbilitySystem(PlayerState->GetWitchForestASC());
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
