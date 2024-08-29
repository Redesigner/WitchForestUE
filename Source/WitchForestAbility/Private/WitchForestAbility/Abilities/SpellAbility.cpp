// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/SpellAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/MovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Logging/StructuredLog.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"
#include "WitchForestAbility/Effects/EffectApplicationComponent.h"
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
		SpawnProjectile(ActorInfo->AvatarActor->GetActorLocation());
	}

	// CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);
	CommitExecute(Handle, ActorInfo, ActivationInfo);

	if (bEndAbilityOnFire)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USpellAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USpellAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	/*
	UGameplayEffect* GameplayEffect = GetCooldownGameplayEffect();
	if (GameplayEffect && (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)))
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, GameplayEffect->GetClass(), GetAbilityLevel(Handle, ActorInfo));
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetStackCount(1);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
	*/
}


void USpellAbility::SpawnProjectile(FVector Location)
{
	if (!ProjectileClass)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellAbility '{AbilityName}' failed to spawn projectile. The projectile class was invalid.", GetName());
		return;
	}

	FQuat SpawnRotation;
	if (CurrentActorInfo->AvatarActor.IsValid())
	{
		SpawnRotation = CurrentActorInfo->AvatarActor->GetActorQuat();
	}

	FTransform ProjectileTransform;
	ProjectileTransform.SetLocation(Location);
	ProjectileTransform.SetRotation(SpawnRotation);
	ASpellProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASpellProjectile>(ProjectileClass.Get(), ProjectileTransform);

	if (UEffectApplicationComponent* EffectApplier = Projectile->GetComponentByClass<UEffectApplicationComponent>())
	{
		EffectApplier->SetEffectHandles(CurrentActorInfo->AvatarActor.Get(), EffectContainer.MakeEffectSpecs(this));
	}
	Projectile->FinishSpawning(ProjectileTransform);
	Projectile->SetVelocity(CurrentActorInfo->AvatarActor->GetActorForwardVector() * ProjectileSpeed);
}
