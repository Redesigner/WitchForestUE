// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Character/Witch/Components/WitchMovementComponent.h"

#include "AbilitySystemInterface.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

UWitchMovementComponent::UWitchMovementComponent()
{
}

void UWitchMovementComponent::PhysWalking(float DeltaTime, int32 Iterations)
{
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->HasMatchingGameplayTag(WitchForestGameplayTags::GameplayEffect_Immobile))
	{
		return;
	}

	Super::PhysWalking(DeltaTime, Iterations);
}

void UWitchMovementComponent::SetASC(UWitchForestASC* ASC)
{
	AbilitySystemComponent = ASC;
}

FRotator UWitchMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->HasMatchingGameplayTag(WitchForestGameplayTags::GameplayEffect_Immobile))
	{
		return FRotator();
	}

	return Super::GetDeltaRotation(DeltaTime);
}

float UWitchMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return GetMaxWalkSpeed();
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

float UWitchMovementComponent::GetMaxWalkSpeed() const
{
	const float Dot = FMath::Clamp(UpdatedComponent->GetForwardVector().Dot(Velocity.GetSafeNormal2D()), 0.0f, 1.0f);
	return FMath::Lerp(MaxWalkSpeedCrouched, MaxWalkSpeed, Dot);
}