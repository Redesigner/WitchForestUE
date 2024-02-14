// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/EnemyAIController.h"

#include "WitchForestGame/Character/Enemies/Enemy.h"
#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

#include "Navigation/PathFollowingComponent.h" 
#include "GameFramework/PlayerState.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (!Enemy)
	{
		return;
	}
	EnemyPawn = Enemy;
	Enemy->OnTakeDamage.AddDynamic(this, &AEnemyAIController::DamageReceived);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	FVector Destination;
	if (!TryGetDestination(Destination))
	{
		return;
	}
	FPathFollowingRequestResult Result = MoveTo(Destination);
	if (Result == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed movement request."));
	}
}

void AEnemyAIController::DamageReceived(AActor* Source, FHitResult Hit)
{
	if (!Target.IsValid())
	{
		AActor* NewTarget = Source;
		if (APlayerState* TargetPlayerState = Cast<APlayerState>(Source))
		{
			NewTarget = TargetPlayerState->GetPawn();
		}
		Target = NewTarget;
	}
}

bool AEnemyAIController::TryGetDestination(FVector& Destination)
{
	if (!Target.IsValid())
	{
		return false;
	}

	const FVector TargetLocation = Target->GetActorLocation();
	const FVector TargetDelta = TargetLocation - EnemyPawn->GetActorLocation();
	const FVector TargetDirection = TargetDelta.GetSafeNormal2D();
	const float TargetDistanceSquared = TargetDelta.SquaredLength();
	float DesiredDistance = MaxDistance;

	const bool bWithinRadius = TargetDistanceSquared < MaxDistance * MaxDistance;
	EnemyPawn->GetEnemyMovementComponent()->SetOrientRotationToMovement(!bWithinRadius);
	ControlRotation = FRotator(0.0f, FMath::RadiansToDegrees(atan2(TargetDirection.Y, TargetDirection.X)), 0.0f);

	if (bWithinRadius)
	{
		// If the target is not closer than our MinDistance, we don't need to move
		if (TargetDistanceSquared >= MinDistance * MinDistance)
		{
			Destination = EnemyPawn->GetActorLocation();
			return true;
		}
		// otherwise, move so we are at least MinDistance away from the target
		DesiredDistance = MinDistance;
	}
	const FVector TargetOffset = TargetDirection * DesiredDistance;
	Destination = TargetLocation - TargetOffset;
	return true;
}
