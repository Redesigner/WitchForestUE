// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "Components/CapsuleComponent.h"


UEnemyMovementComponent::UEnemyMovementComponent()
{
	RequestedVelocity = FVector::Zero();
	PendingImpulses = FVector::Zero();
}


void UEnemyMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PhysMovement(DeltaTime);
}


void UEnemyMovementComponent::SetUpdatedComponent(USceneComponent* Component)
{
	Super::SetUpdatedComponent(Component);

	EnemyOwner = Cast<AEnemy>(PawnOwner);
}


void UEnemyMovementComponent::AddImpulse(FVector Impulse)
{
	// DrawDebugDirectionalArrow(GetWorld(), PawnOwner->GetActorLocation(), PawnOwner->GetActorLocation() + Impulse, 25.0f, FColor::Red, false, 2.0f);
	PendingImpulses += Impulse;
}


void UEnemyMovementComponent::SetMovementMode(EEnemyMovementMode NewMovementMode)
{
	MovementMode = NewMovementMode;
}


void UEnemyMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	SetDefaultMovementMode();
}


#if WITH_EDITOR
void UEnemyMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEnemyMovementComponent, MaxFloorWalkableAngle))
	{
		MaxFloorWalkableZ = FMath::Sin(FMath::DegreesToRadians(MaxFloorWalkableAngle));
	}
}
#endif


void UEnemyMovementComponent::HandleBlockingImpact(FHitResult ImpactHitResult)
{
	if (ImpactHitResult.IsValidBlockingHit())
	{
		float Dot = Velocity.Dot(ImpactHitResult.ImpactNormal);
		if (Dot < 0.0f)
		{
			Velocity -= Dot * ImpactHitResult.ImpactNormal;
		}

		if (ImpactHitResult.ImpactNormal.Z >= MaxFloorWalkableZ)
		{
			MovementMode = EEnemyMovementMode::MOVE_Walking;
			Basis = ImpactHitResult.GetComponent();
			// EnemyOwner->AddActorWorldOffset(FVector(0.0f, 0.0f, 5.0f));
		}
	}
}


void UEnemyMovementComponent::PhysMovement(float DeltaTime)
{
	switch (MovementMode)
	{
	case(EEnemyMovementMode::MOVE_None):
		break;

	case(EEnemyMovementMode::MOVE_Walking):
		PhysWalking(DeltaTime);
		break;

	case(EEnemyMovementMode::MOVE_Falling):
		PhysFalling(DeltaTime);
		break;
	}
}


void UEnemyMovementComponent::PhysFalling(float DeltaTime)
{
	Velocity += FVector(0.0f, 0.0f, GetWorld()->GetGravityZ() * DeltaTime);
	Velocity += ConsumeImpulses();
	FHitResult MoveHitResult;
	SafeMoveUpdatedComponent(Velocity * DeltaTime, UpdatedComponent->GetComponentRotation(), true, MoveHitResult);

	if (MoveHitResult.IsValidBlockingHit())
	{
		HandleBlockingImpact(MoveHitResult);
	}
}


void UEnemyMovementComponent::PhysWalking(float DeltaTime)
{
	const FVector Normal2D = Velocity.GetSafeNormal2D();
	const FVector PlanarVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);
	FVector FrictionDelta = Normal2D * Friction;
	float EffectiveMaxSpeed = MaxSpeed;

	/*
	// If we aren't orienting our rotation to movement, then for now, assume we are strafing and limit the speed
	if (!bOrientRotationToMovement)
	{
		EffectiveMaxSpeed = MaxStrafeSpeed;
	}
	*/

	// Limit the friction so that it doesn't cause the pawn to accelerate backwards -- this isn't how friction works in real life!
	if (FrictionDelta.SquaredLength() > PlanarVelocity.SquaredLength())
	{
		FrictionDelta = PlanarVelocity;
	}
	Velocity -= FrictionDelta;

	// Ignore our inputs here if we have root motion
	// if (!EnemyOwner->GetSkeletalMeshComponent()->IsPlayingRootMotion())
	{
		Velocity += Acceleration * ConsumeInputVector();
		// ApplyRootMotionToVelocity(DeltaTime);

		if (!RequestedVelocity.IsNearlyZero())
		{
			// Treat our requested velocity as a directional vector, since it can be a very large number
			FVector VelocityDelta = RequestedVelocity - Velocity;
			VelocityDelta.Z = 0;
			Velocity += VelocityDelta.GetUnsafeNormal() * Acceleration;
		}
	}

	Velocity = Velocity.GetClampedToMaxSize2D(EffectiveMaxSpeed);

	// Apply any impulses we have after we've applied the friction
	FVector Impulses = ConsumeImpulses();
	Velocity += Impulses;
	ApplyRootMotionToVelocity(DeltaTime);

	FHitResult MoveHitResult;
	const FVector DeltaLocation = Velocity * DeltaTime;
	const float FloorDotDelta = (BasisNormal | DeltaLocation);
	FVector RampMovement(DeltaLocation.X, DeltaLocation.Y, -FloorDotDelta / BasisNormal.Z);

	SafeMoveUpdatedComponent(RampMovement, UpdatedComponent->GetComponentRotation(), true, MoveHitResult);

	UCapsuleComponent* CapsuleComponent = EnemyOwner->GetCapsule();
	// If we hit a wall here, see if we can step over it, by doing a capsule cast down from our desired location
	if (MoveHitResult.IsValidBlockingHit())
	{
		const FVector DesiredLocation = CapsuleComponent->GetComponentLocation() + DeltaLocation;
		FHitResult StepUpHitResult;
		const float StepUpHeight = 15.0f;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(PawnOwner);

		GetWorld()->SweepSingleByChannel(StepUpHitResult, DesiredLocation + FVector::UpVector * StepUpHeight, DesiredLocation, CapsuleComponent->GetComponentQuat(),
			CapsuleComponent->GetCollisionObjectType(), CapsuleComponent->GetCollisionShape(1.0f), CollisionQueryParams);
		if (StepUpHitResult.IsValidBlockingHit())
		{
			const FVector StepUpDelta = StepUpHitResult.Location - CapsuleComponent->GetComponentLocation();
			PawnOwner->AddActorWorldOffset(StepUpDelta);
		}
		// HandleBlockingImpact(MoveHitResult);
	}
	SetDefaultMovementMode();
	UpdateRotation(DeltaTime);
}


void UEnemyMovementComponent::UpdateRotation(float DeltaTime)
{
	const FVector Normal2D = Velocity.GetSafeNormal2D();

	if (Normal2D.IsNearlyZero() && bOrientRotationToMovement)
	{
		return;
	}
	FRotator CurrentRotation = PawnOwner->GetActorRotation();
	const float DesiredYaw = FMath::RadiansToDegrees(FMath::Atan2(Normal2D.Y, Normal2D.X));
	const float DeltaYaw = FMath::FInterpConstantTo(CurrentRotation.Yaw, DesiredYaw, DeltaTime, RotationSpeed);
	FRotator DesiredRotation = CurrentRotation;
	DesiredRotation.Yaw = bOrientRotationToMovement ? DesiredYaw : EnemyOwner->GetControlRotation().Yaw;
	FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, DesiredRotation, DeltaTime, RotationSpeed);
	PawnOwner->SetActorRotation(NewRotation);
}


FVector UEnemyMovementComponent::ConsumeImpulses()
{
	FVector Impulses = PendingImpulses;
	PendingImpulses = FVector::Zero();
	return Impulses;
}


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

	// const bool bValidHit = GetWorld()->LineTraceSingleByChannel(OutHitResult, CapsuleLocation, CapsuleLocation - FVector::UpVector * (FloorSnapDistance + Capsule->GetScaledCapsuleHalfHeight()),
		// Capsule->GetCollisionObjectType(), CollisionQueryParams);

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
	// DrawDebugDirectionalArrow(GetWorld(), PawnOwner->GetActorLocation(), PawnOwner->GetActorLocation() + RequestedVelocity.GetSafeNormal() * 300.0f, 25.0f, FColor::Blue, false, -1.0f);
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


bool UEnemyMovementComponent::IsFalling() const
{
	return MovementMode == EEnemyMovementMode::MOVE_Falling;
}


void UEnemyMovementComponent::ApplyRootMotionToVelocity(float DeltaTime)
{
	USkeletalMeshComponent* Mesh = EnemyOwner->GetSkeletalMesh();
	if (!Mesh)
	{
		return;
	}
	FRootMotionMovementParams RootMotionParams = Mesh->ConsumeRootMotion();
	// Animation root motion is distinct from root motion sources right now and takes precedence
	if (Mesh->IsPlayingRootMotion() && RootMotionParams.bHasRootMotion && DeltaTime > 0.f)
	{
		FTransform DeltaTransformFromRootMotion = Mesh->ConvertLocalRootMotionToWorld(RootMotionParams.GetRootMotionTransform());
		Velocity += DeltaTransformFromRootMotion.GetLocation() / DeltaTime;
		return;
	}
}


void UEnemyMovementComponent::SetOrientRotationToMovement(bool bValue)
{
	bOrientRotationToMovement = bValue;
}
