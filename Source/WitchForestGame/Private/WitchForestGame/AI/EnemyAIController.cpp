// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/EnemyAIController.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Character/Enemies/Enemy.h"
#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

#include "Navigation/PathFollowingComponent.h" 
#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h" 

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
	else
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "AIController '{SelfName}' failed to set a behavior tree for Pawn '{PawnName}'. It does not have a valid BehaviorTree Asset set.", GetName(), InPawn->GetName());
	}

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
	Super::Tick(DeltaTime);

	APawn* OwnerPawn = GetPawn();
	if (!OwnerPawn || !TargetActor.IsValid())
	{
		return;
	}
	const float DistanceSquared = (TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation()).SquaredLength();
	bool bInRange = DistanceSquared <= MinimumRange * MinimumRange;
	SetFocus(TargetActor.Get());

	if (Blackboard->GetValueAsBool("InRange") != bInRange)
	{
		Blackboard->SetValueAsBool("InRange", bInRange);
	}
}

void AEnemyAIController::DamageReceived(AActor* Source, FHitResult Hit)
{
	AActor* Target = Source;
	if (APlayerState* TargetPlayerState = Cast<APlayerState>(Source))
	{
		Target = TargetPlayerState->GetPawn();
	}
	SetTarget(Target);
}

void AEnemyAIController::SetTarget(AActor* Target)
{
	TargetActor = Target;
	// Blackboard->SetValueAsObject(TargetKey.SelectedKeyName, Target);
	Blackboard->SetValueAsObject("TargetActor", Target);
}
