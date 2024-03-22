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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation", meta = (AllowPrivateAccess = true))
	float RotationSpeed = 360.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotation", meta = (AllowPrivateAccess = true))
	bool bOrientRotationToMovement = true;


	FVector RequestedVelocity;

	FVector PendingImpulses;
	

public:
	UEnemyMovementComponent();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AddImpulse(FVector Impulse);

	void SetMovementMode(EEnemyMovementMode Mode);

	void SetOrientRotationToMovement(bool bValue);

	void SetMaxSpeed(float NewMaxSpeed);


protected:
	void BeginPlay() override;

	void SetUpdatedComponent(USceneComponent* Component) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


	virtual void HandleBlockingImpact(FHitResult ImpactHitResult);

	virtual void PhysMovement(float DeltaTime);

	virtual void PhysFalling(float DeltaTime);

	virtual void PhysWalking(float DeltaTime);

	void UpdateRotation(float DeltaTime);

	FVector ConsumeImpulses();

	virtual void SetDefaultMovementMode();

	virtual bool FindFloor(FHitResult& OutHitResult) const;

	virtual bool SnapToFloor();

	bool IsFalling() const;

	void ApplyRootMotionToVelocity(float DeltaTime);


	//  BEGIN INTERFACE Navigation Movement component
	void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

	void RequestPathMove(const FVector& MoveInput) override;

	bool CanStartPathFollowing() const override;

	bool CanStopPathFollowing() const override;

	void StopActiveMovement() override;
	// END INTERFACE
};
