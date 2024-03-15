// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Logging/StructuredLog.h"

#include "WitchForestAbility.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ASpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	AddActorWorldOffset(Velocity * DeltaTime, true, &HitResult);
	if (HitResult.IsValidBlockingHit())
	{
		ApplyGameplayEffectToTarget(HitResult.GetActor(), HitResult);
	}
}

void ASpellProjectile::SetEffectHandle(FGameplayEffectSpecHandle InHandle)
{
	EffectHandle = InHandle;
}

void ASpellProjectile::SetOwningActor(AActor* Actor)
{
	OwningActor = Actor;
}

void ASpellProjectile::SetVelocity(FVector NewVelocity)
{
	Velocity = NewVelocity;
}

void ASpellProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor == OwningActor)
	{
		return;
	}

	// Generate a fake hit with our projectile's velocity as our normal
	// We don't necessarily have a PrimitiveComponent, so this will be null
	FVector WorldVelocity = Velocity;
	FHitResult FakeHit = FHitResult(OtherActor, nullptr, GetActorLocation(), -WorldVelocity.GetSafeNormal());
	ApplyGameplayEffectToTarget(OtherActor, FakeHit);
}

void ASpellProjectile::ApplyGameplayEffectToTarget(AActor* Target, FHitResult HitResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!EffectHandle.IsValid())
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "Projectile {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. The GameplayEffectHandle was invalid. Make sure to call SetEffectHandle after spawning the projectile.", GetName(), Target->GetName());
		return;
	}

	if (ActorsHit.Contains(Target))
	{
		return;
	}

	if (IAbilitySystemInterface* ActorAbility = Cast<IAbilitySystemInterface>(Target))
	{
		FGameplayEffectContextHandle EffectContext = EffectHandle.Data->GetContext();
		// We have to reset the effectcontext's hit results, otherwise we may be
		// trying to apply effects with hit results that have fallen out of scope,
		// since internally they are stored as sharedptrs
		EffectContext.AddHitResult(HitResult, true);

		ActorsHit.Add(Target);
		
		if (!ActorAbility->GetAbilitySystemComponent())
		{
			UE_LOGFMT(LogWitchForestAbility, Warning, "Projectile {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. The owning ASC of this projectile was invalid.", GetName(), Target->GetName());
			return;
		}

		FGameplayEffectSpec* Spec = EffectHandle.Data.Get();
		if (!Spec)
		{
			return;
		}

		ActorAbility->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

