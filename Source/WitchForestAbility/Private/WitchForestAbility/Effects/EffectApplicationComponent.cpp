// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Effects/EffectApplicationComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"

#include "WitchForestAbility.h"

void UEffectApplicationComponent::SetEffectHandles(AActor* OwningActorIn, TArray<FGameplayEffectSpecHandle> InHandles)
{
	OwningActor = OwningActorIn;
	EffectHandles = InHandles;
}

TArray<FActiveGameplayEffectHandle> UEffectApplicationComponent::ApplyGameplayEffectsToTarget(AActor* Target, FHitResult HitResult)
{
	TArray<FActiveGameplayEffectHandle> Result;

	if (!GetOwner()->HasAuthority() || !Target)
	{
		return Result;
	}

	if (EffectHandles.Num() == 0 && DefaultEffects.Num() == 0)
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "EffectApplicationComponent {ComponentName} was unable to apply a GameplayEffect to {OtherActorName}. No GameplayEffectHandles were set. Make sure to call SetEffectHandles after spawning the projectile.", GetName(), Target->GetName());
		return Result;
	}

	if (IAbilitySystemInterface* ActorAbility = Cast<IAbilitySystemInterface>(Target))
	{
		for (FGameplayEffectSpecHandle EffectHandle : EffectHandles)
		{
			FGameplayEffectContextHandle EffectContext = EffectHandle.Data->GetContext();
			// We have to reset the effectcontext's hit results, otherwise we may be
			// trying to apply effects with hit results that have fallen out of scope,
			// since internally they are stored as sharedptrs
			EffectContext.AddHitResult(HitResult, true);
			EffectContext.AddOrigin(OwningActor->GetActorLocation());

			FGameplayEffectSpec* Spec = EffectHandle.Data.Get();
			if (!Spec)
			{
				continue;
			}


			if (!ActorAbility->GetAbilitySystemComponent())
			{
				// We have to check each application that the ASC is valid.
				// If, for instance, an applied effect kills the actor, the
				// ASC could be deleted or otherwise invalidated
				UE_LOGFMT(LogWitchForestAbility, Warning, "Area of Effect {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. The target ASC was invalid.", GetName(), Target->GetName());
				return Result;
			}

			Result.Add(ActorAbility->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec));
		}

		for (TSubclassOf<UGameplayEffect> GameplayEffectClass : DefaultEffects)
		{
			if (!ActorAbility->GetAbilitySystemComponent())
			{
				// We have to check each application that the ASC is valid.
				// If, for instance, an applied effect kills the actor, the
				// ASC could be deleted or otherwise invalidated
				UE_LOGFMT(LogWitchForestAbility, Warning, "Area of Effect {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. The target ASC was invalid.", GetName(), Target->GetName());
				return Result;
			}

			if (!GameplayEffectClass)
			{
				UE_LOGFMT(LogWitchForestAbility, Warning, "Area of Effect {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. The GameplayEffectClass was invalid.", GetName(), Target->GetName());
				continue;
			}

			FGameplayEffectContextHandle ContextHandle = ActorAbility->GetAbilitySystemComponent()->MakeEffectContext();
			Result.Add(ActorAbility->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(GameplayEffectClass->GetDefaultObject()), 1.0f, ContextHandle));
		}
	}
	return Result;
}
