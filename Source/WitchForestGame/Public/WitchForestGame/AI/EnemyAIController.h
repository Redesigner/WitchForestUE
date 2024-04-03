// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "BehaviorTree/BehaviorTreeTypes.h" 
#include "Perception/AIPerceptionTypes.h"

#include "EnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;
class UAIPerceptionComponent;

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

	TWeakObjectPtr<AActor> TargetActor;


	AEnemyAIController();

	UFUNCTION()
	void TargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

	UFUNCTION()
	void TargetPerceptionForgotten(AActor* Actor);

	void OnPossess(APawn* InPawn) override;

	void Tick(float DeltaTime) override;

	void OnDeath();

	void SetTarget(AActor* Target);

	void ClearTarget();


	// IGenericTeamAgentInterface
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// End IGenericTeamAgentInterface

	// Begin IWitchForestTeamAgentInterface
	void OverrideTeam(EWitchForestTeam NewTeam) override;
	void SetWitchForestTeam(EWitchForestTeam InTeam) override;
	EWitchForestTeam GetWitchForestTeam() const override;
	// End IWitchForestTeamAgentInterface
};
