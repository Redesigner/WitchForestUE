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
#include "Perception/AIPerceptionComponent.h"
#include "GenericTeamAgentInterface.h"



AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	AIPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::TargetPerceptionUpdated);
}

void AEnemyAIController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Stimulus.IsActive() || !Actor)
	{
		return;
	}

	SetTarget(Actor);
}

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
	Enemy->OnDeath.AddUObject(this, &ThisClass::OnDeath);
	Blackboard->SetValueAsBool("Alive", true);
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
	const float TargetDistance = sqrtf(DistanceSquared);
	// SetFocus(TargetActor.Get());

	Blackboard->SetValueAsFloat("TargetDistance", TargetDistance);
}

void AEnemyAIController::OnDeath()
{
	Blackboard->SetValueAsBool("Alive", false);
	// Clear our target to make sure we don't look at it anymore
	SetTarget(nullptr);
}

void AEnemyAIController::SetTarget(AActor* Target)
{
	TargetActor = Target;
	// Blackboard->SetValueAsObject(TargetKey.SelectedKeyName, Target);
	Blackboard->SetValueAsObject("TargetActor", Target);
}

FGenericTeamId AEnemyAIController::GetGenericTeamId() const
{
	return TeamId;
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			if (OtherTeamAgent->GetGenericTeamId() == GetGenericTeamId())
			{
				return ETeamAttitude::Friendly;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}
	}

	return ETeamAttitude::Neutral;
}
