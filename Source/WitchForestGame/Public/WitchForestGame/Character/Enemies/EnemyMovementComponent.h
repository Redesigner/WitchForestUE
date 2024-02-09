// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "EnemyMovementComponent.generated.h"

class AEnemy;

UENUM(BlueprintType)
enum class EEnemyMovementMode : uint8
{
	MOVE_None,
	MOVE_Walking,
	MOVE_Falling
};

UCLASS()
class WITCHFORESTGAME_API UEnemyMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	TWeakObjectPtr<AEnemy> EnemyOwner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = true))
	EEnemyMovementMode MovementMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = true))
	float MaxSpeed = 300.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = true))
	float Friction = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = true))
	float Acceleration = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Falling", meta = (AllowPrivateAccess = true))
	float FloorSnapDistance = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Floor", meta = (AllowPrivateAccess = true, ClampMax = 90.0f, ClampMin = 0.0f))
	float MaxFloorWalkableAngle = 35.0f;
	float MaxFloorWalkableZ = 0.573576f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement|Floor", meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UPrimitiveComponent> Basis;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement|Floor", meta = (AllowPrivateAccess = true))
	FVector BasisNormal;

	FVector RequestedVelocity;
	
public:
	virtual void SetDefaultMovementMode();

	virtual bool FindFloor(FHitResult& OutHitResult) const;

	virtual bool SnapToFloor();

	// Navigation Movement component interface
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	virtual void RequestPathMove(const FVector& MoveInput) override;
	virtual bool CanStartPathFollowing() const override;
	virtual bool CanStopPathFollowing() const override;
	virtual void StopActiveMovement() override;
};
