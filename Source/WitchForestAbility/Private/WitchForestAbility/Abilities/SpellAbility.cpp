// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/SpellAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/MovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Logging/StructuredLog.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

void USpellAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ProjectileClass)
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "SpellAbility '{AbilityName}' failed to spawn projectile, Projectile Class is invalid.", GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellAbility '{AbilityName}' failed to spawn projectile, world is invalid.", GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ActorInfo->AvatarActor.IsValid())
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "SpellAbility '{AbilityName}' failed to spawn projectile '{ProjectileName}', AbilitySystemComponent does not have a valid AvatarActor.", GetName(), ProjectileClass->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Only spawn the projectile on the server, and replicate out from there
	if (ActorInfo->IsNetAuthority())
	{
		FTransform ProjectileTransform = ActorInfo->AvatarActor->GetActorTransform();
		ASpellProjectile* Projectile = World->SpawnActorDeferred<ASpellProjectile>(ProjectileClass.Get(), ProjectileTransform);
		// Projectile->SetReplicates(true);
		FGameplayEffectSpecHandle NewEffectSpec = MakeOutgoingGameplayEffectSpec(SpellEffect);
		if (!NewEffectSpec.IsValid())
		{
			UE_LOGFMT(LogWitchForestAbility, Warning, "SpellAbility '{AbilityName}' failed to create spec of GameplayEffect '{GameplayEffectName}'.", GetName(), SpellEffect->GetName());
		}
		else
		{
			FGameplayEffectSpec* Spec = NewEffectSpec.Data.Get();
			if (Spec)
			{
				Spec->SetSetByCallerMagnitude(WitchForestGameplayTags::SetByCaller_Damage, -DamageAmount);
			}

			Projectile->SetEffectHandle(NewEffectSpec);
			Projectile->SetOwningActor(ActorInfo->AvatarActor.Get());
			Projectile->FinishSpawning(ProjectileTransform);
			FVector ProjectileVelocity = ActorInfo->AvatarActor->GetActorForwardVector() * 800.0f;
			Projectile->SetVelocity(ProjectileVelocity);
		}
	}

	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);
}

void USpellAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}