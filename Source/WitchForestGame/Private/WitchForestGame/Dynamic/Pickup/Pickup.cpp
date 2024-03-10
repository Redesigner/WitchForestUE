// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

#include "WitchForestAbility/WitchForestASC.h"

void APickup::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsFake)
    {
        if (!FakeOwner.IsValid())
        {
            SetLifeSpan(0.1f);
            return;
        }

        FVector Delta = FakeOwner->GetActorLocation() - GetActorLocation();
        float SquaredLength = Delta.SquaredLength();
        if (SquaredLength <= 1.0f)
        {
            FakeOwner->SetActorHiddenInGame(false);
            Destroy();
            return;
        }

        if (SquaredLength >= 50000.0f)
        {
            // UE_LOG(LogTemp, Warning, TEXT("Pickup fake simulation was too far away from replicated position, teleporting."));
            DrawDebugSphere(GetWorld(), GetActorLocation(), CollisionSphere->GetScaledSphereRadius(), 8, FColor::Red, false, 0.5f);
            DrawDebugSphere(GetWorld(), FakeOwner->GetActorLocation(), CollisionSphere->GetScaledSphereRadius(), 8, FColor::Blue, false, 0.5f);
            DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), FakeOwner->GetActorLocation(), 5.0f, FColor::Blue, false, 0.5f);

            SetActorLocation(FakeOwner->GetActorLocation());
            SetVelocity(FakeOwner->GetVelocity());
            return;
        }

        AddActorWorldOffset(Delta * FMath::Clamp(DeltaSeconds, 0.0f, 1.0f));
    }
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
    PrimaryActorTick.bCanEverTick = true;
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

void APickup::AttachFakeTo(APickup* Other)
{
    bIsFake = true;
    SetActorTickEnabled(true);
    FakeOwner = Other;
}
