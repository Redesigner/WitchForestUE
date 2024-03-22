// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Pickup/PickupMovementComponent.h"

#include "Net/UnrealNetwork.h"


UPickupMovementComponent::UPickupMovementComponent()
{
	SetIsReplicatedByDefault(true);
}


void UPickupMovementComponent::OnRep_PickupMovement()
{
	if (!UpdatedComponent)
	{
		return;
	}

	//UE_LOG(LogTemp, Display, TEXT("Replicated pickup movement."));

	UpdatedComponent->ComponentVelocity = PreviousMovement.Velocity;

	FVector Delta = PreviousMovement.Location - UpdatedComponent->GetComponentLocation();
	DrawDebugDirectionalArrow(GetWorld(), PreviousMovement.Location, PreviousMovement.Location + PreviousMovement.Velocity, 10.0f, FColor::Red, false, 0.5f);

	UpdatedComponent->MoveComponent(Delta, UpdatedComponent->GetComponentRotation(), false);
}

/*
void UPickupMovementComponent::OnRep_PreviousLocation()
{
	if (!UpdatedComponent)
	{
		return;
	}
	// DrawDebugSphere(GetWorld(), UpdatedComponent->GetComponentLocation(), 50.0f, 16, FColor::Red, false, 1.0f);
	FVector Delta = PreviousLocation - UpdatedComponent->GetComponentLocation();
	UpdatedComponent->MoveComponent(Delta, UpdatedComponent->GetComponentRotation(), false);
}
*/

void UPickupMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UpdatedComponent)
	{
		return;
	}

	if (!bActiveMovement)
	{
		return;
	}

	FVector NewVelocity = UpdatedComponent->GetComponentVelocity();

	if (!bOnFloor)
	{
		NewVelocity += FVector(0.0f, 0.0f, GetWorld()->GetGravityZ() * DeltaTime);
	}
	else
	{
		// Apply friction?
	}

	const FVector Delta = DeltaTime * NewVelocity;

	FHitResult HitResult;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		const FVector AbsorbedVelocity = HitResult.Normal.Dot(NewVelocity) * HitResult.Normal;
		NewVelocity -= bBounce ? AbsorbedVelocity * (1.0f + BounceRatio) : AbsorbedVelocity;

		if (NewVelocity.Z <= 10.0f && HitResult.Normal.Z >= 0.95f)
		{
			NewVelocity.Z = 0.0f;
			// bOnFloor = true;
		}
	}

	UpdatedComponent->ComponentVelocity = NewVelocity;
	FPickupReplicatedMovement NewMovement;
	NewMovement.Velocity = NewVelocity;
	NewMovement.Location = UpdatedComponent->GetComponentLocation();
	PreviousMovement = NewMovement;

	// PreviousLocation = UpdatedComponent->GetComponentLocation();
}

void UPickupMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, PreviousMovement, COND_SkipOwner);
	// DOREPLIFETIME_CONDITION(ThisClass, PreviousLocation, COND_SkipOwner);
}

void UPickupMovementComponent::SetVelocity(FVector InVelocity)
{
	if (!UpdatedComponent)
	{
		return;
	}

	UpdatedComponent->ComponentVelocity = InVelocity;
}
