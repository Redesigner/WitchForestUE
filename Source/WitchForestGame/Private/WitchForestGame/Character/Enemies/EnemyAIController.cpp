// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/EnemyAIController.h"

#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "Navigation/PathFollowingComponent.h" 

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (!Enemy)
	{
		return;
	}
	EnemyPawn = Enemy;
	Enemy->OnTakeDamage.BindUObject(this, &AEnemyAIController::DamageReceived);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	FVector Destination;
	if (!TryGetDestination(Destination))
	{
		return;
	}
	MoveTo(Destination);
}

void AEnemyAIController::DamageReceived(AActor* Source)
{
	if (!Target.IsValid())
	{
		Target = Source;
	}
}

bool AEnemyAIController::TryGetDestination(FVector& Destination)
{
	if (!Target.IsValid())
	{
		return false;
	}

	const FVector TargetDelta = Target->GetActorLocation() - EnemyPawn->GetActorLocation();
	const FVector TargetDirection = TargetDelta.GetSafeNormal2D();
	const float TargetDistanceSquared = TargetDelta.SquaredLength();
	float DesiredDistance = MaxDistance;

	float ModifiedMaxDistance = MaxDistance - 35.0f;

	if (TargetDistanceSquared < MaxDistance * MaxDistance)
	{
		// If the target is not closer than our min distance, we don't need to move
		if (TargetDistanceSquared >= ModifiedMaxDistance * ModifiedMaxDistance)
		{
			return false;
		}
		// otherwise, move so we are at least mindistance away from the target
		DesiredDistance = MinDistance;
	}
	const FVector TargetOffset = TargetDirection * DesiredDistance;
	Destination = Target->GetActorLocation() - TargetOffset;
	return true;
}
