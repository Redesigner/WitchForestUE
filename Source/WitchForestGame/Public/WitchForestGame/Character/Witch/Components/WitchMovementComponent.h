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

	TWeakObjectPtr<UWitchForestASC> AbilitySystemComponent;


	UWitchMovementComponent();

	void PhysWalking(float DeltaTime, int32 Iterations) override;

	FRotator GetDeltaRotation(float DeltaTime) const override;

	float GetMaxSpeed() const override;

	float GetMaxWalkSpeed() const;

public:
	/// Pass in the ASC so we can check it for relevant tags, etc.
	void SetASC(UWitchForestASC* ASC);
};