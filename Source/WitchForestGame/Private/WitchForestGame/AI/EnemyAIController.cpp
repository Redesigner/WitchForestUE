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
	AIPerception->OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &ThisClass::TargetPerceptionInfoUpdated);
	AIPerception->OnTargetPerceptionForgotten.AddUniqueDynamic(this, &ThisClass::TargetPerceptionForgotten);
}

void AEnemyAIController::TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	UE_LOGFMT(LogWitchForestAI, Verbose, "AI '{SelfName}' received perception update: Sensed target '{TargetName}' with '{UpdateType}' '{IsActive}', '{IsExpired}'.", GetName(),
		UpdateInfo.Target.IsValid() ? UpdateInfo.Target->GetName(): "Null",
		UpdateInfo.Stimulus.Type.Name,
		UpdateInfo.Stimulus.IsActive() ? "Active" : "Inactive",
		UpdateInfo.Stimulus.IsExpired() ? "Expired" : "Not Expired");

	if (!UpdateInfo.Target.IsValid())
	{
		UE_LOGFMT(LogWitchForestAI, Verbose, "AI '{SelfName}' recieved perception update, but the perceived target was invalid.", GetName());
		return;
	}

	if (UpdateInfo.Stimulus.IsActive())
	{
		SetTarget(UpdateInfo.Target.Get());
	}
	else if (!AIPerception->HasAnyActiveStimulus(*UpdateInfo.Target.Get()) )
	{
		UE_LOGFMT(LogWitchForestAI, Verbose, "AI '{SelfName}' did not have any other stimulus for '{TargetName}', clearing target.", GetName(), UpdateInfo.Target->GetName());
		ClearTarget();
	}
}

void AEnemyAIController::TargetPerceptionForgotten(AActor* Actor)
{
	UE_LOGFMT(LogWitchForestAI, Verbose, "AI '{SelfName}' forgot target '{TargetName}'.", GetName(), Actor ? Actor->GetName() : "Null");
	if (TargetActor == Actor)
	{
		ClearTarget();
	}
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
	if (Target == TargetActor)
	{
		return;
	}

	if (!Target)
	{
		// If you're trying to set the target to nullptr intentionally, use ClearTarget() instead.
		return;
	}

	TargetActor = Target;
	Blackboard->SetValueAsObject("TargetActor", Target);
}

void AEnemyAIController::ClearTarget()
{
	TargetActor = nullptr;
	Blackboard->ClearValue("TargetActor");
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IWitchForestTeamAgentInterface* OtherTeamAgent = Cast<IWitchForestTeamAgentInterface>(OtherPawn->GetController()))
		{
			if (OtherTeamAgent->GetWitchForestTeam() == GetWitchForestTeam())
			{
				// UE_LOGFMT(LogWitchForestGame, Display, "EnemyAIController '{SelfName}' attitude towards '{OtherName}' is friendly.", GetName(), Other.GetName());
				// UE_LOGFMT(LogWitchForestGame, Display, "'' My team is '{TeamName}', perceived target is team '{OtherTeamName}' ''.", UEnum::GetDisplayValueAsText(GetWitchForestTeam()).ToString(), UEnum::GetDisplayValueAsText(OtherTeamAgent->GetWitchForestTeam()).ToString());
				return ETeamAttitude::Friendly;
			}
			else
			{
				// UE_LOGFMT(LogWitchForestGame, Display, "EnemyAIController '{SelfName}' attitude towards '{OtherName}' is hostile.", GetName(), Other.GetName());
				// UE_LOGFMT(LogWitchForestGame, Display, "'' My team is '{TeamName}', perceived target is team '{OtherTeamName}'. ''", UEnum::GetDisplayValueAsText(GetWitchForestTeam()).ToString(), UEnum::GetDisplayValueAsText(OtherTeamAgent->GetWitchForestTeam()).ToString());
				return ETeamAttitude::Hostile;
			}
		}
	}

	return ETeamAttitude::Neutral;
}

void AEnemyAIController::OverrideTeam(EWitchForestTeam NewTeam)
{
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
	if (!PerceptionSystem)
	{
		return;
	}

	if (NewTeam != GetWitchForestTeam())
	{
		ClearTarget();
	}

	PerceptionSystem->UnregisterSource(*this);
	PerceptionSystem->UnregisterListener(*PerceptionComponent.Get());

	if (NewTeam == EWitchForestTeam::Unaffiliated)
	{
		bOverridingTeam = false;
	}
	else
	{
		bOverridingTeam = true;
		OverridenTeam = NewTeam;
	}

	PerceptionSystem->RegisterSource(*this);
	PerceptionSystem->UpdateListener(*PerceptionComponent.Get());

	BrainComponent->RestartLogic();
}

void AEnemyAIController::SetWitchForestTeam(EWitchForestTeam InTeam)
{
	Team = InTeam;
}

EWitchForestTeam AEnemyAIController::GetWitchForestTeam() const
{
	return bOverridingTeam ? OverridenTeam : Team;
}
