// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/UseKeyAbility.h"

#include "Components/SphereComponent.h"

#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchFOrestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Dynamic/World/LockedDoor.h"

void UUseKeyAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor.Get());
	if (!Witch)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	APickup* HeldItem = ItemHandle->GetHeldItem();
	if (!HeldItem)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (!HeldItem->HasMatchingGameplayTag(RequiredItemTag))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	UPrimitiveComponent* InteractionVolume = Witch->GetInteractionVolume();
	if (!InteractionVolume)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	TSet<AActor*> OverlappingActors;
	InteractionVolume->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (ALockedDoor* LockedDoor = Cast<ALockedDoor>(OverlappingActor))
		{
			LockedDoor->Unlock();
			ItemHandle->ConsumeItem()->Destroy();
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

bool UUseKeyAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor.Get());
	if (!Witch)
	{
		return false;
	}

	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		return false;
	}

	UPrimitiveComponent* InteractionVolume = Witch->GetInteractionVolume();
	if (!InteractionVolume)
	{
		return false;
	}

	if (!ItemHandle->GetHeldItem())
	{
		return false;
	}

	if (!ItemHandle->GetHeldItem()->HasMatchingGameplayTag(RequiredItemTag))
	{
		return false;
	}

	TSet<AActor*> OverlappingActors;
	InteractionVolume->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (ALockedDoor* Door = Cast<ALockedDoor>(OverlappingActor))
		{
			return true;
		}
	}

	return false;
}
