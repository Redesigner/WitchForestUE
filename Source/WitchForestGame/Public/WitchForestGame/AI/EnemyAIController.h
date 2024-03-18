// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h" 
#include "Perception/AIPerceptionTypes.h"

#include "EnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;
class UAIPerceptionComponent;

UCLASS()
class WITCHFORESTGAME_API AEnemyAIController : public AAIController
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
	FGenericTeamId TeamId;


	TWeakObjectPtr<AEnemy> EnemyPawn;

	TWeakObjectPtr<AActor> TargetActor;


	AEnemyAIController();

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void OnPossess(APawn* InPawn) override;

	void Tick(float DeltaTime) override;

	void OnDeath();

	void SetTarget(AActor* Target);


	// IGenericTeamAgentInterface
	FGenericTeamId GetGenericTeamId() const override;
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// End IGenericTeamAgentInterface
};
