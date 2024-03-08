// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

#include "WitchForestAbility/WitchForestASC.h"

void APickup::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void APickup::OnRep_ReplicatedMovement()
{
    Super::OnRep_ReplicatedMovement();

    LastReplicatedPosition = GetActorLocation();
    LastReplicatedVelocity = GetVelocity();
}

void APickup::GatherCurrentMovement()
{
    /*
    if (GetOwner() && GetOwner()->GetLocalRole() > ENetRole::ROLE_SimulatedProxy)
    {
        // UE_LOG(LogTemp, Display, TEXT("Ignoring replication because this client has assumed ownership of the object."))
        return;
    }
    */
    Super::GatherCurrentMovement();
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

void APickup::RestoreLastReplicatedMovement()
{
    SetActorLocation(LastReplicatedPosition);
    SetVelocity(LastReplicatedVelocity);
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

void APickup::ServerRequestReplicatedMovementUpdate_Implementation()
{
    ClientForceReplicatedMovementUpdate(GetActorLocation(), GetVelocity());
}

void APickup::ClientForceReplicatedMovementUpdate_Implementation(FVector Location, FVector Velocity)
{
    SetActorLocation(Location);
    SetVelocity(Velocity);
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
