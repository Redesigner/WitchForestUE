// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "PickupMovementComponent.generated.h"


USTRUCT(BlueprintType)
struct FPickupReplicatedMovement
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;
};

UCLASS()
class WITCHFORESTGAME_API UPickupMovementComponent : public UMovementComponent
{
	GENERATED_BODY()


	UPROPERTY(ReplicatedUsing = OnRep_PickupMovement)
	FPickupReplicatedMovement PreviousMovement;

	// UPROPERTY(ReplicatedUsing = OnRep_PreviousLocation)
	// FVector PreviousLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bBounce = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Movement, meta = (AllowPrivateAccess = true, ClampMin = 0.0f, ClampMax = 0.0f))
	float BounceRatio = 0.5f;

	bool bOnFloor = false;

	bool bActiveMovement = true;



	UFUNCTION()
	void OnRep_PickupMovement();

	// UFUNCTION()
	// void OnRep_PreviousLocation();


	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPickupMovementComponent();

	void SetActiveMovement(bool bInActive) { bActiveMovement = bInActive; }

	void SetVelocity(FVector InVelocity);
};
