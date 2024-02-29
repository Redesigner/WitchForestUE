// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/SpellAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

void USpellAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ProjectileClass)
	{
		UE_LOG(LogWitchForestAbility, Warning, TEXT("Failed to spawn projectile, Projectile Class is invalid."));
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AActor* OwnerActor = ActorInfo->OwnerActor.Get();
	if (APlayerState* PlayerState = Cast<APlayerState>(OwnerActor))
	{
		OwnerActor = PlayerState->GetPawn();
	}
	FTransform ProjectileTransform = OwnerActor->GetActorTransform();
	ASpellProjectile* Projectile = World->SpawnActorDeferred<ASpellProjectile>(ProjectileClass.Get(), ProjectileTransform);
	Projectile->SetEffectHandle(MakeOutgoingGameplayEffectSpec(SpellEffect));
	Projectile->SetOwningActor(OwnerActor);
	Projectile->FinishSpawning(ProjectileTransform);
	FVector ProjectileVelocity = Projectile->GetVelocity() + OwnerActor->GetVelocity();
	// Projectile->SetVe
	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);
}

void USpellAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}