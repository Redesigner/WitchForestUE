// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/PickupItemAbility.h"

#include "WitchForestAbility.h"
#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "Components/SphereComponent.h"
#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"

void UPickupItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor);
	if (!Witch)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	USphereComponent* InteractVolume = Witch->GetInteractionVolume();
	if (!ItemHandle || !InteractVolume)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TSet<AActor*> OverlappingActors;
	InteractVolume->GetOverlappingActors(OverlappingActors, APickup::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		APickup* OverlappingPickup = Cast<APickup>(OverlappingActor);
		if (!OverlappingPickup || OverlappingPickup->bHeld)
		{
			continue;
		}

		PickupItem(OverlappingPickup, ItemHandle);

		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}


bool UPickupItemAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Check normal activation first, this is **VERY** important

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// We can now do our own logic here

	// A lot of these checks are going to be repeated in ActivateAbility, but since it's dependent
	// on certain components existing, there isn't a way around it
	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor);
	if (!Witch)
	{
		// For now, only witches can use this ability, because it requires an interaction volume.
		// In the future, this should be an interface or something similar
		return false;
	}

	// Make sure we have all of our necessary components, and we aren't carrying anything
	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	USphereComponent* InteractVolume = Witch->GetInteractionVolume();
	if (!ItemHandle || ItemHandle->HoldingItem() || !InteractVolume)
	{
		return false;
	}

	// Check that we have at least one pickup that is not being held by someone else
	TSet<AActor*> OverlappingActors;
	InteractVolume->GetOverlappingActors(OverlappingActors, APickup::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		APickup* OverlappingPickup = Cast<APickup>(OverlappingActor);
		if (!OverlappingPickup || OverlappingPickup->bHeld)
		{
			continue;
		}
		return true;
	}
	return false;
}

void UPickupItemAbility::ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey)
{
	if (!ActorInfo->AvatarActor.IsValid())
	{
		return;
	}

	UItemHandleComponent* ItemHandle = ActorInfo->AvatarActor->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		return;
	}

	if (ItemHandle->FakePickup.IsValid())
	{
		ItemHandle->FakePickup->Destroy();
	}

	if (ItemHandle->RequestedPickup.IsValid())
	{
		ItemHandle->RequestedPickup->bHeld = false;
		ItemHandle->RequestedPickup->SetActorHiddenInGame(false);
		ItemHandle->RequestedPickup.Reset();
	}
}

void UPickupItemAbility::ActivateAbilitySucceed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FPredictionKey& PredictionKey)
{

	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor);
	if (!Witch)
	{
		return;
	}

	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		return;
	}

	if (!ItemHandle->RequestedPickup.IsValid() || !ItemHandle->RequestedPickup.IsValid())
	{
		return;
	}

	ItemHandle->FakePickup->Destroy();
	ItemHandle->PickupItem(ItemHandle->RequestedPickup.Get());
	ItemHandle->RequestedPickup->SetOwner(Witch);
	ItemHandle->RequestedPickup->SetActorHiddenInGame(false);

	ItemHandle->RequestedPickup.Reset();
	ItemHandle->FakePickup.Reset();
}

void UPickupItemAbility::PickupItem(APickup* Item, UItemHandleComponent* ItemHandle)
{
	if (IsPredictingClient())
	{
		SimulatePickupItem(Item, ItemHandle);
	}
	else
	{
		ServerPickupItem(Item, ItemHandle);
	}
}

void UPickupItemAbility::SimulatePickupItem(APickup* Item, UItemHandleComponent* ItemHandle)
{
	ItemHandle->FakePickup = GetWorld()->SpawnActorDeferred<APickup>(Item->GetClass(), Item->GetTransform());
	if (!ItemHandle->FakePickup.Get())
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "PickupItemAbility '{AbilityName}' failed to spawn fake pickup actor from template pickup '{PickupName}'.", GetName(), Item->GetName());
		return;
	}
	ItemHandle->FakePickup->SetReplicates(false);
	ItemHandle->FakePickup->FinishSpawning(Item->GetActorTransform());

	// ItemHandle->PickupItem(FakePickup);
	ItemHandle->FakePickup->DisableMovement();
	ItemHandle->FakePickup->AttachToComponent(ItemHandle, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	ItemHandle->RequestedPickup = Item;
	ItemHandle->RequestedPickup->bHeld = true;
	ItemHandle->RequestedPickup->SetActorHiddenInGame(true);
}

void UPickupItemAbility::ServerPickupItem(APickup* Item, UItemHandleComponent* ItemHandle)
{
	ItemHandle->PickupItem(Item);
	// Item->SetOwner(ItemHandle->GetOwner());
}
