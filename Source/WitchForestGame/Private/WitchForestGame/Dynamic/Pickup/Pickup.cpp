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
    if (HasLocalNetOwner())
    {
        return;
    }

    // DrawDebugSphere(GetWorld(), GetReplicatedMovement().Location, 32.0f, 8, FColor::Blue, false, 0.5f);

    Super::OnRep_ReplicatedMovement();
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

void APickup::ServerRequestChangeOwner_Implementation(AActor* NewOwner)
{
    ClientChangeOwner(NewOwner);
}

void APickup::ClientChangeOwner_Implementation(AActor* NewOwner)
{
    SetOwner(NewOwner);
}

void APickup::ServerSetLocationAndVelocity_Implementation(FVector Location, FVector Velocity, AActor* InstigatorAvatar)
{
    /*if (!bHeld)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to set location and velocity. Pickup was not being held by an actor"));
        return;
    }

    if (!IsAttachedTo(InstigatorAvatar))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to set location and velocity. Pickup was not being held by this actor"));
        return;
    }*/

    DrawDebugSphere(GetWorld(), Location, 32.0f, 8, FColor::Blue, false, 0.5f);
    DrawDebugSphere(GetWorld(), GetActorLocation(), 32.0f, 8, FColor::Red, false, 0.5f);

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
