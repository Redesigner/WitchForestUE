// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/InteractAbility.h"

#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Display/TimerDisplay.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"
#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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
		if (OverlappingActor->Implements<UInteractableInterface>())
		{
			// float HoldTime = Interactable->GetRequiredHoldTime();
			float HoldTime = IInteractableInterface::Execute_GetRequiredHoldTime(OverlappingActor);
			UE_LOGFMT(LogWitchForestAbility, Display, "[{ClientServer}] InteractAbility triggered, required hold time '{HoldTime}s'.", IsPredictingClient() ? "Client" : "Server", HoldTime);
			if (HoldTime <= 0.0f)
			{
				IInteractableInterface::Execute_Interact(OverlappingActor, Witch);
				// Interactable->Interact(Witch);
				EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			}
			else
			{
				StartHoldTimer(TScriptInterface<IInteractableInterface>(OverlappingActor), Handle, ActorInfo, ActivationInfo);
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
		if (OverlappingActor->Implements<UInteractableInterface>())
		{
			return IInteractableInterface::Execute_CanInteract(OverlappingActor, Witch);
			// return Interactable->CanInteract(Witch);
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

	if (HoldTimerDisplay.IsValid())
	{
		HoldTimerDisplay->Destroy();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UInteractAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOGFMT(LogWitchForestAbility, Display, "InteractAbility cancelled by input release. [{ClientServer}]", IsPredictingClient() ? "Client" : "Server");
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UInteractAbility::StartHoldTimer(TScriptInterface<IInteractableInterface> InteractableTarget, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// This should always be an AActor
	Target = Cast<AActor>(InteractableTarget.GetObject());
	GetWorld()->GetTimerManager().SetTimer(HoldTimer, FTimerDelegate::CreateUObject(this, &ThisClass::EndHoldTimer), IInteractableInterface::Execute_GetRequiredHoldTime(InteractableTarget.GetObject()), false, -1.0f);
	
	if (!IsPredictingClient())
	{
		if (HeldEffectClass)
		{
			FGameplayEffectSpecHandle HeldEffect = MakeOutgoingGameplayEffectSpec(HeldEffectClass);
			HeldEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, HeldEffect);
		}
	}
	if (ActorInfo->IsLocallyControlledPlayer())
	{
		HoldTimerDisplay = GetWorld()->SpawnActor <ATimerDisplay>(HoldTimerDisplayClass);
		HoldTimerDisplay->SetActorLocation(Cast<AActor>(InteractableTarget.GetObject())->GetActorLocation());
		HoldTimerDisplay->SetTimerHandle(HoldTimer);
	}
}

void UInteractAbility::EndHoldTimer()
{
	// The timer is set on both the client and server, but it's only use for our visual component on the client
	if (IsPredictingClient())
	{
		return;
	}

	// This is a workaround for the failure of TWeakInterfacePtr to work with Blueprint implemented interfaces
	if (Target.IsValid() && Target.Get()->Implements<UInteractableInterface>() && CurrentActorInfo->AvatarActor.IsValid())
	{
		IInteractableInterface::Execute_Interact(Target.Get(), CurrentActorInfo->AvatarActor.Get());
	}

	UE_LOGFMT(LogWitchForestAbility, Display, "InteractAbility completed by timer end. [{ClientServer}]", IsPredictingClient() ? "Client" : "Server");
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
