// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

#include "WitchForestAbility/WitchForestASC.h"

void APickup::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

APickup::APickup()
{
    bReplicates = true;
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
}

void APickup::EnableMovement()
{
    CollisionSphere->SetSimulatePhysics(true);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APickup::DisableMovement()
{
    CollisionSphere->SetSimulatePhysics(false);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickup::ServerSetLocationAndVelocity_Implementation(FVector Location, FVector Velocity, AActor* InstigatorAvatar)
{
    if (!bHeld)
    {
        return;
    }

    if (!IsAttachedTo(InstigatorAvatar))
    {
        return;
    }

    SetActorLocation(Location);
    CollisionSphere->SetPhysicsLinearVelocity(Velocity);
}

void APickup::AddImpulse(FVector Impulse)
{
    if (!CollisionSphere)
    {
        return;
    }
    CollisionSphere->AddImpulse(Impulse);
}

void APickup::SetVelocity(FVector Velocity)
{
    CollisionSphere->SetPhysicsLinearVelocity(Velocity);
}

UWitchForestAbilitySet* APickup::GetGrantedAbilitySet() const
{
    return GrantedAbilitySet;
}

void APickup::SetLastHeldASC(UAbilitySystemComponent* ASC)
{
    LastHolder = ASC;
}
