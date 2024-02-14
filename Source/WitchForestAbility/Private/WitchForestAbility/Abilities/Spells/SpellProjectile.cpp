// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Logging/StructuredLog.h"

#include "WitchForestAbility.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	AddActorLocalOffset(Velocity * DeltaTime, true, &HitResult);
	if (HitResult.IsValidBlockingHit())
	{
		ApplyGameplayEffectToTarget(HitResult.GetActor(), HitResult);
	}
}

void ASpellProjectile::SetEffectHandle(FGameplayEffectSpecHandle InHandle)
{
	EffectHandle = InHandle;
}

void ASpellProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Generate a fake hit with our projectile's velocity as our normal
	// We don't necessarily have a PrimitiveComponent, so this will be null
	FVector WorldVelocity = GetActorRotation().RotateVector(Velocity);
	FHitResult FakeHit = FHitResult(OtherActor, nullptr, GetActorLocation(), -WorldVelocity.GetSafeNormal());
	ApplyGameplayEffectToTarget(OtherActor, FakeHit);
}

void ASpellProjectile::ApplyGameplayEffectToTarget(AActor* Target, FHitResult HitResult)
{
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
		EffectContext.AddHitResult(HitResult);

		ActorsHit.Add(Target);
		ActorAbility->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectHandle.Data.Get());
	}
}

