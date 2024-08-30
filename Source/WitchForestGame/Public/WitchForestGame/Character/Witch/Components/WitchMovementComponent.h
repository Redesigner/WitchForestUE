// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "WitchMovementComponent.generated.h"

class UWitchForestASC;

UCLASS()
class WITCHFORESTGAME_API UWitchMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Wandering, meta = (AllowPrivateAccess = true))
	float MaxWalkSpeedWandering = 100.0f;


	TWeakObjectPtr<UWitchForestASC> AbilitySystemComponent;

	bool bWandering = false;


	UWitchMovementComponent();

	void SetUpdatedComponent(USceneComponent* Component) override;

	void PhysWalking(float DeltaTime, int32 Iterations) override;

	FRotator GetDeltaRotation(float DeltaTime) const override;
	void PhysicsRotation(float DeltaTime) override;

	float GetMaxSpeed() const override;

	float GetMaxWalkSpeed() const;

public:
	void SetASC(UWitchForestASC* ASC);

	void SetWandering(bool Wandering);

};