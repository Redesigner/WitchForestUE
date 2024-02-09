// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "Components/CapsuleComponent.h"

void UEnemyMovementComponent::SetDefaultMovementMode()
{
	if (SnapToFloor())
	{
		MovementMode = EEnemyMovementMode::MOVE_Walking;
		return;
	}
	MovementMode = EEnemyMovementMode::MOVE_Falling;
}


bool UEnemyMovementComponent::FindFloor(FHitResult& OutHitResult) const
{
	if (!GetWorld())
	{
		return false;
	}
	// Only find the floor if we are falling
	if (Velocity.Z > KINDA_SMALL_NUMBER)
	{
		return false;
	}
	const UCapsuleComponent* Capsule = EnemyOwner->GetCapsule();
	const FVector CapsuleLocation = Capsule->GetComponentLocation();
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PawnOwner);

	const bool bValidHit = GetWorld()->SweepSingleByChannel(OutHitResult, CapsuleLocation, CapsuleLocation - FVector::UpVector * FloorSnapDistance,
		Capsule->GetComponentQuat(), Capsule->GetCollisionObjectType(), Capsule->GetCollisionShape(1.0f), CollisionQueryParams);

	if (bValidHit && OutHitResult.ImpactNormal.Z >= MaxFloorWalkableZ)
	{
		return true;
	}
	return false;
}


bool UEnemyMovementComponent::SnapToFloor()
{
	FHitResult HitResult;
	const bool bFoundFloor = FindFloor(HitResult);

	if (bFoundFloor)
	{
		Basis = HitResult.GetComponent();
		// Aka: If they aren't nearly equal
		if (!(BasisNormal - HitResult.ImpactNormal).IsNearlyZero())
		{
			BasisNormal = HitResult.ImpactNormal;
			const FVector Delta = HitResult.Location - EnemyOwner->GetCapsule()->GetComponentLocation();
			// Apply the "snap" to our actor, just for safety
			EnemyOwner->AddActorWorldOffset(Delta);
		}
	}

	return bFoundFloor;
}


void UEnemyMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	RequestedVelocity = MoveVelocity;
}

void UEnemyMovementComponent::RequestPathMove(const FVector& MoveInput)
{
	Super::RequestPathMove(MoveInput);
}

bool UEnemyMovementComponent::CanStartPathFollowing() const
{
	return true;
}

bool UEnemyMovementComponent::CanStopPathFollowing() const
{
	return !IsFalling();
}

void UEnemyMovementComponent::StopActiveMovement()
{
	RequestedVelocity = FVector::Zero();
}