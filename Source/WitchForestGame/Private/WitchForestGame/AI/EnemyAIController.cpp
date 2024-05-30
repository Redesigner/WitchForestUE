// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/EnemyAIController.h"

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Enemies/Enemy.h"
#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

#include "Navigation/PathFollowingComponent.h" 
#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h" 
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemComponent.h"


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

	if (UpdateInfo.Stimulus.IsActive())
	{
		// SetTarget(UpdateInfo.Target.Get());
		// SetDesiredLocation(UpdateInfo.Stimulus.StimulusLocation);
		SetAIMovementState(EEnemyAIMovementState::Following);
	}
}

void AEnemyAIController::TargetPerceptionForgotten(AActor* Actor)
{
	if (!AIPerception)
	{
		return;
	}

	TArray<AActor*> PerceivedActors;
	AIPerception->GetCurrentlyPerceivedActors(nullptr, PerceivedActors);
	// We're still perceiving some actors, so don't do anything
	if (!PerceivedActors.IsEmpty())
	{
		return;
	}
	AIPerception->GetKnownPerceivedActors(nullptr, PerceivedActors);
	if (PerceivedActors.IsEmpty())
	{
		// If we forget a perception, and don't remember any actors, switch to wandering
		SetAIMovementState(EEnemyAIMovementState::Wandering);
	}
	else
	{
		// Find out closest remaining perception

	}
}

void AEnemyAIController::OnTakeDamage(AActor* Source, FHitResult Hit)
{
	if (Team == EWitchForestTeam::Passive)
	{
		UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(GetWorld());
		if (!PerceptionSystem)
		{
			return;
		}

		PerceptionSystem->UnregisterSource(*this);
		PerceptionSystem->UnregisterListener(*PerceptionComponent.Get());

		Team = EWitchForestTeam::Wild;

		PerceptionSystem->RegisterSource(*this);
		PerceptionSystem->UpdateListener(*PerceptionComponent.Get());
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
	Enemy->OnTakeDamage.AddUniqueDynamic(this, &ThisClass::OnTakeDamage);

	Blackboard->SetValueAsBool("Alive", true);

	UAbilitySystemComponent* EnemyASC = Enemy->GetAbilitySystemComponent();
	if (!EnemyASC)
	{
		return;
	}

	EnemyASC->RegisterGameplayTagEvent(WitchForestGameplayTags::GameplayEffect_Blind, EGameplayTagEventType::AnyCountChange).AddUObject(this, &ThisClass::BlindStacksChanged);
	EnemyASC->RegisterGameplayTagEvent(WitchForestGameplayTags::GameplayEffect_Sight, EGameplayTagEventType::AnyCountChange).AddUObject(this, &ThisClass::BlindStacksChanged);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* OwnerPawn = GetPawn();
	if (!OwnerPawn || MovementState != EEnemyAIMovementState::Following)
	{
		return;
	}

	UpdateClosestTarget();
}

void AEnemyAIController::OnDeath()
{
	Blackboard->SetValueAsBool("Alive", false);
}

void AEnemyAIController::SetDesiredLocation(FVector Location)
{
	TargetLocation = Location;
	Blackboard->SetValueAsVector("TargetLocation", Location);
}

void AEnemyAIController::SetAIMovementState(EEnemyAIMovementState State)
{
	MovementState = State;
	Blackboard->SetValueAsEnum("MovementState", static_cast<uint8>(State));
}

void AEnemyAIController::BlindStacksChanged(const FGameplayTag Tag, int32 Count)
{
	if (!PerceptionComponent || !EnemyPawn.IsValid())
	{
		return;
	}

	// This should never happen since we're being called by the ASC, but just to make sure
	UAbilitySystemComponent* EnemyASC = EnemyPawn->GetAbilitySystemComponent();
	if (!EnemyASC)
	{
		return;
	}

	const int32 StacksBlind = EnemyASC->GetGameplayTagCount(WitchForestGameplayTags::GameplayEffect_Blind);
	const int32 StacksSight = EnemyASC->GetGameplayTagCount(WitchForestGameplayTags::GameplayEffect_Sight);

	PerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), StacksSight >= StacksBlind);
}

void AEnemyAIController::UpdateClosestTarget()
{
	if (!AIPerception || !GetPawn())
	{
		return;
	}

	if (MovementState != EEnemyAIMovementState::Following)
	{
		return;
	}

	const FVector CurrentLocation = GetPawn()->GetActorLocation();
	FVector PerceptionLocation;
	bool bFoundClosestPerceivedActor = false;
	float ClosestDistanceSquared = 0.0f;
	AActor* ClosestTarget = nullptr;

	TArray<AActor*> CurrrentlyPerceivedActors;
	AIPerception->GetCurrentlyPerceivedActors(nullptr, CurrrentlyPerceivedActors);

	if (CurrrentlyPerceivedActors.IsEmpty())
	{
		// We can't perceive any actors right now, but we might remember some
		SetAIMovementState(EEnemyAIMovementState::Investigating);
		const FActorPerceptionInfo* PerceptionInfo = AIPerception->GetFreshestTrace(UAISense::GetSenseID(UAISense_Sight::StaticClass()));
		if (PerceptionInfo)
		{
			SetDesiredLocation(PerceptionInfo->GetLastStimulusLocation());
		}
		return;
	}

	for (AActor* CurrentlyPerceivedActor : CurrrentlyPerceivedActors)
	{
		FActorPerceptionBlueprintInfo PerceptionInfo;
		if (AIPerception->GetActorsPerception(CurrentlyPerceivedActor, PerceptionInfo))
		{
			for (const FAIStimulus& Stimulus : PerceptionInfo.LastSensedStimuli)
			{
				if (Stimulus.IsValid())
				{
					PerceptionLocation = Stimulus.StimulusLocation;
					break;
				}
			}
		}
		const float CurrentDistanceSquared = (PerceptionLocation - CurrentLocation).SquaredLength();
		if (!bFoundClosestPerceivedActor || CurrentDistanceSquared < ClosestDistanceSquared)
		{
			bFoundClosestPerceivedActor = true;
			ClosestTarget = CurrentlyPerceivedActor;
			ClosestDistanceSquared = CurrentDistanceSquared;
		}
	}
	
	// UE_LOGFMT(LogWitchForestAI, Warning, "EnemyAIController '{AIController}' Perception at location '{Location}'", GetName(), PerceptionLocation.ToString());

	Blackboard->SetValueAsVector("TargetLocation", PerceptionLocation);
	Blackboard->SetValueAsObject("TargetActor", ClosestTarget);
}


ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IWitchForestTeamAgentInterface* OtherTeamAgent = Cast<IWitchForestTeamAgentInterface>(OtherPawn->GetController()))
		{
			if (GetWitchForestTeam() == EWitchForestTeam::Passive)
			{
				return ETeamAttitude::Neutral;
			}
			if (OtherTeamAgent->GetWitchForestTeam() == GetWitchForestTeam())
			{
				// UE_LOGFMT(LogWitchForestGame, Display, "EnemyAIController '{SelfName}' attitude towards '{OtherName}' is friendly.", GetName(), Other.GetName());
				// UE_LOGFMT(LogWitchForestGame, Display, "'' My team is '{TeamName}', perceived target is team '{OtherTeamName}' ''.", UEnum::GetDisplayValueAsText(GetWitchForestTeam()).ToString(), UEnum::GetDisplayValueAsText(OtherTeamAgent->GetWitchForestTeam()).ToString());
				return ETeamAttitude::Friendly;
			}
			if (OtherTeamAgent->GetWitchForestTeam() == EWitchForestTeam::Passive && GetWitchForestTeam() == EWitchForestTeam::Wild)
			{
				// If we're wild and the other enemy is passive, assume we *used* to be on the same team
				return ETeamAttitude::Friendly;
			}
			return ETeamAttitude::Hostile;
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

	if (PerceptionComponent)
	{
		PerceptionComponent->ForgetAll();
	}

	if (BrainComponent->GetBlackboardComponent())
	{
		BrainComponent->GetBlackboardComponent()->ClearValue("TargetActor");
	}
	SetAIMovementState(EEnemyAIMovementState::Wandering);
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
