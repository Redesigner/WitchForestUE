// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/InteractAbility.h"

#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "Components/SphereComponent.h"

void UInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor);
	if (!Witch)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	USphereComponent* InteractionVolume = Witch->GetInteractionVolume();
	if (!InteractionVolume)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	TSet<AActor*> OverlappingActors;
	InteractionVolume->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(OverlappingActor))
		{
			Interactable->Interact(Witch);
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

bool UInteractAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	AWitch* Witch = Cast<AWitch>(ActorInfo->AvatarActor);
	if (!Witch)
	{
		return false;
	}

	USphereComponent* InteractionVolume = Witch->GetInteractionVolume();
	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	if (!InteractionVolume || !ItemHandle || ItemHandle->HoldingItem())
	{
		return false;
	}

	TSet<AActor*> OverlappingActors;
	InteractionVolume->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(OverlappingActor))
		{
			return Interactable->CanInteract(Witch);
		}
	}

	return false;
}
