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
}

void AEnemyAIController::TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (!UpdateInfo.Stimulus.IsActive() || !UpdateInfo.Target.IsValid())
	{
		return;
	}

	SetTarget(UpdateInfo.Target.Get());
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

	UE_LOGFMT(LogWitchForestGame, Display, "EnemyAIController '{SelfName}' set target to '{TargetName}'.", GetName(), Target ? Target->GetName() : "Null");
	TargetActor = Target;
	// Blackboard->SetValueAsObject(TargetKey.SelectedKeyName, Target);
	Blackboard->SetValueAsObject("TargetActor", Target);
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IWitchForestTeamAgentInterface* OtherTeamAgent = Cast<IWitchForestTeamAgentInterface>(OtherPawn->GetController()))
		{
			if (OtherTeamAgent->GetWitchForestTeam() == GetWitchForestTeam())
			{
				UE_LOGFMT(LogWitchForestGame, Display, "EnemyAIController '{SelfName}' attitude towards '{OtherName}' is friendly.", GetName(), Other.GetName());
				UE_LOGFMT(LogWitchForestGame, Display, "'' My team is '{TeamName}', perceived target is team '{OtherTeamName}' ''.", UEnum::GetDisplayValueAsText(GetWitchForestTeam()).ToString(), UEnum::GetDisplayValueAsText(OtherTeamAgent->GetWitchForestTeam()).ToString());
				return ETeamAttitude::Friendly;
			}
			else
			{
				UE_LOGFMT(LogWitchForestGame, Display, "EnemyAIController '{SelfName}' attitude towards '{OtherName}' is hostile.", GetName(), Other.GetName());
				UE_LOGFMT(LogWitchForestGame, Display, "'' My team is '{TeamName}', perceived target is team '{OtherTeamName}'. ''", UEnum::GetDisplayValueAsText(GetWitchForestTeam()).ToString(), UEnum::GetDisplayValueAsText(OtherTeamAgent->GetWitchForestTeam()).ToString());
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
		SetTarget(nullptr);
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
