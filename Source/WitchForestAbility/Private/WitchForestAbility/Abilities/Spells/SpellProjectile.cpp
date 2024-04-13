// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

#include "WitchForestAbility.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ASpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAffectedByGravity)
	{
		Velocity.Z += GetWorld()->GetGravityZ() * DeltaTime;
	}

	FHitResult HitResult;
	AddActorWorldOffset(Velocity * DeltaTime, true, &HitResult);
	
	if (HitResult.bBlockingHit && bDestroyOnBlockingHit)
	{
		Destroy();
	}
}

void ASpellProjectile::SetEffectHandles(TArray<FGameplayEffectSpecHandle> InHandles)
{
	EffectHandles = InHandles;
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
	ApplyGameplayEffectsToTarget(OtherActor, FakeHit);
}

void ASpellProjectile::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}

void ASpellProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Velocity);
}

void ASpellProjectile::ApplyGameplayEffectsToTarget(AActor* Target, FHitResult HitResult)
{
	if (!HasAuthority() || !Target)
	{
		return;
	}

	if (EffectHandles.Num() == 0)
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "Projectile {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. No GameplayEffectHandles were set. Make sure to call SetEffectHandles after spawning the projectile.", GetName(), Target->GetName());
		return;
	}

	if (ActorsHit.Contains(Target))
	{
		return;
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

			ActorsHit.Add(Target);

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
				UE_LOGFMT(LogWitchForestAbility, Warning, "Projectile {ProjectileName} was unable to apply a GameplayEffect to {OtherActorName}. The owning ASC of this projectile was invalid.", GetName(), Target->GetName());
				return;
			}

			ActorAbility->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec);
		}
	}
}

