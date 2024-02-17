// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h" 

#include "EnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;

UCLASS()
class WITCHFORESTGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = BlackBoard, meta = (AllowPrivateAccess = true))
	FBlackboardKeySelector TargetKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Behavior, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Behavior, meta = (AllowPrivateAccess = true))
	float MinimumRange = 500.0f;

	TWeakObjectPtr<AEnemy> EnemyPawn;

	TWeakObjectPtr<AActor> TargetActor;

public:
	void OnPossess(APawn* InPawn) override;

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void DamageReceived(AActor* Source, FHitResult Hit);

private:
	void SetTarget(AActor* Target);
};
