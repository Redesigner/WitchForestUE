// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Effects/EffectApplicationComponent.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	EffectApplication = CreateDefaultSubobject<UEffectApplicationComponent>(TEXT("EffectApplication"));
}

void ASpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAffectedByGravity)
	{
		Velocity.Z += GetWorld()->GetGravityZ() * DeltaTime;
	}

	FHitResult HitResult;
	AddActorWorldOffset(Velocity * DeltaTime, true, &HitResult, ETeleportType::None);
	
	if (HitResult.bBlockingHit && !HitResult.GetActor()->IsA<APawn>())
	{
		HitWall(HitResult);
	}

	if (HitResult.bBlockingHit && bDestroyOnBlockingHit)
	{
		Destroy();
	}
}

void ASpellProjectile::SetVelocity(FVector NewVelocity)
{
	Velocity = NewVelocity;
}

void ASpellProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (OtherActor == EffectApplication->GetOwningActor())
	{
		return;
	}

	// Generate a fake hit with our projectile's velocity as our normal
	// We don't necessarily have a PrimitiveComponent, so this will be null
	FVector WorldVelocity = Velocity;
	FHitResult FakeHit = FHitResult(OtherActor, nullptr, GetActorLocation(), -WorldVelocity.GetSafeNormal());

	if (!ActorsHit.Contains(OtherActor))
	{
		EffectApplication->ApplyGameplayEffectsToTarget(OtherActor, FakeHit);
		ActorsHit.Add(OtherActor);
	}

	if (OtherActor->Implements<UAbilitySystemInterface>() && bDestroyOnEffectApplied)
	{
		Destroy();
	}
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
