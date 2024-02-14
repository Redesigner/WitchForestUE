// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemy;

UCLASS()
class WITCHFORESTGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Targeting|Walking", meta = (AllowPrivateAccess = true))
	float MaxDistance = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Targeting|Walking", meta = (AllowPrivateAccess = true))
	float MinDistance = 100.0f;

	TWeakObjectPtr<AActor> Target;

	TWeakObjectPtr<AEnemy> EnemyPawn;

public:
	void OnPossess(APawn* InPawn) override;

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void DamageReceived(AActor* Source, FHitResult Hit);

	bool TryGetDestination(FVector& Destination);
};
