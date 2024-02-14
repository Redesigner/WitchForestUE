// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpellProjectile::SetEffectHandle(FGameplayEffectSpecHandle InHandle)
{
	EffectHandle = InHandle;
}

void ASpellProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ApplyGameplayEffectToTarget(OtherActor);
}

void ASpellProjectile::ApplyGameplayEffectToTarget(AActor* Target)
{
}

