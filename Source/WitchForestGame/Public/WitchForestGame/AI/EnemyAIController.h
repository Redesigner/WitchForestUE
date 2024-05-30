// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "BehaviorTree/BehaviorTreeTypes.h" 
#include "Perception/AIPerceptionTypes.h"
#include "GameplayTagContainer.h"

#include "EnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;
class UAIPerceptionComponent;

UENUM(BlueprintType)
enum class EEnemyAIMovementState : uint8
{
	Wandering,
	Investigating,
	Following
};

UCLASS()
class WITCHFORESTGAME_API AEnemyAIController : public AAIController, public IWitchForestTeamAgentInterface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = BlackBoard, meta = (AllowPrivateAccess = true))
	FBlackboardKeySelector TargetKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Behavior, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Behavior, meta = (AllowPrivateAccess = true))
	float MinimumRange = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Behavior, meta = (AllowPrivateAccess = true))
	TEnumAsByte<EWitchForestTeam> Team = EWitchForestTeam::Unaffiliated;
	TEnumAsByte<EWitchForestTeam> OverridenTeam = EWitchForestTeam::Unaffiliated;
	bool bOverridingTeam = false;


	TWeakObjectPtr<AEnemy> EnemyPawn;

	FVector TargetLocation;
	EEnemyAIMovementState MovementState = EEnemyAIMovementState::Wandering;

	AEnemyAIController();

	// AI Perception methods
	UFUNCTION()
	void TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
	UFUNCTION()
	void TargetPerceptionForgotten(AActor* Actor);

	void SetDesiredLocation(FVector Location);

	void SetAIMovementState(EEnemyAIMovementState State);

	void UpdateClosestTarget();
	// End AI Perception

	UFUNCTION()
	void OnTakeDamage(AActor* Source, FHitResult Hit);

	void OnPossess(APawn* InPawn) override;

	void Tick(float DeltaTime) override;

	void OnDeath();

	void BlindStacksChanged(const FGameplayTag Tag, int32 Count);

	// IGenericTeamAgentInterface
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// End IGenericTeamAgentInterface

	// Begin IWitchForestTeamAgentInterface
	void OverrideTeam(EWitchForestTeam NewTeam) override;
	void SetWitchForestTeam(EWitchForestTeam InTeam) override;
	EWitchForestTeam GetWitchForestTeam() const override;
	// End IWitchForestTeamAgentInterface
};
