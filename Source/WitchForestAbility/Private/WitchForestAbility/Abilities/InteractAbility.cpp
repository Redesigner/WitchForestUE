// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/InteractAbility.h"

#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"
#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility.h"

#include "Components/SphereComponent.h"
#include "Logging/StructuredLog.h"

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
			float HoldTime = Interactable->GetRequiredHoldTime();
			// UE_LOGFMT(LogWitchForestAbility, Display, "[{ClientServer}] InteractAbility triggered, required hold time '{HoldTime}s'.", IsPredictingClient() ? "Client" : "Server", HoldTime);
			if (HoldTime <= 0.0f)
			{
				Interactable->Interact(Witch);
				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			}
			else if (!IsPredictingClient())
			{
				Target = Interactable;
				GetWorld()->GetTimerManager().SetTimer(HoldTimer, FTimerDelegate::CreateUObject(this, &ThisClass::EndHoldTimer), HoldTime, false, -1.0f);
				if (HeldEffectClass)
				{
					FGameplayEffectSpecHandle HeldEffect = MakeOutgoingGameplayEffectSpec(HeldEffectClass);
					HeldEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, HeldEffect);
				}
			}
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

void UInteractAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (HeldEffectHandle.IsValid() && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(HeldEffectHandle, 1);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UInteractAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UInteractAbility::EndHoldTimer()
{
	// UE_LOGFMT(LogWitchForestAbility, Display, "InteractAbility hold finished. [{ClientServer}]", IsPredictingClient() ? "Client" : "Server");
	if (Target.IsValid() && CurrentActorInfo->AvatarActor.IsValid())
	{
		Target->Interact(CurrentActorInfo->AvatarActor.Get());
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
