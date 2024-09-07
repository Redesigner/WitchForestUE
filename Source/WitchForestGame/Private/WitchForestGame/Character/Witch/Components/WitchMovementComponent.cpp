// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Character/Witch/Components/WitchMovementComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

UWitchMovementComponent::UWitchMovementComponent()
{
}

void UWitchMovementComponent::SetUpdatedComponent(USceneComponent* Component)
{
	Super::SetUpdatedComponent(Component);

	AbilitySystemComponent = Cast<UWitchForestASC>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

void UWitchMovementComponent::SetASC(UWitchForestASC* ASC)
{
	AbilitySystemComponent = ASC;
}

void UWitchMovementComponent::SetWandering(bool Wandering)
{
	bWandering = Wandering;
}

void UWitchMovementComponent::PhysWalking(float DeltaTime, int32 Iterations)
{
	// Should we be doing this cast ever frame for every actor?
	if (GetOwner())
	{
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
		if (ASI && ASI->GetAbilitySystemComponent() && ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(WitchForestGameplayTags::GameplayEffect_Immobile))
		{
			return;
		}
	}

	Super::PhysWalking(DeltaTime, Iterations);
}

FRotator UWitchMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (GetOwner())
	{
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
		if (ASI && ASI->GetAbilitySystemComponent() && ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(WitchForestGameplayTags::GameplayEffect_Immobile))
		{
			return FRotator();
		}
	}

	return Super::GetDeltaRotation(DeltaTime);
}

void UWitchMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->HasMatchingGameplayTag(WitchForestGameplayTags::GameplayEffect_Immobile))
	{
		return;
	}

	Super::PhysicsRotation(DeltaTime);
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
	float SpeedModifier = 1.0f;

	// @TODO: Use event based changes instead of checking each tick?
	if (AbilitySystemComponent.IsValid())
	{
		if (const UBaseAttributeSet* BaseAttributeSet = Cast<UBaseAttributeSet>(AbilitySystemComponent->GetAttributeSet(UBaseAttributeSet::StaticClass())) )
		{
			SpeedModifier = BaseAttributeSet->GetMovementSpeedModifier();
		}
	}

	if (bWandering)
	{
		return MaxWalkSpeedWandering * SpeedModifier;
	}

	return FMath::Lerp(MaxWalkSpeedCrouched, MaxWalkSpeed, Dot) * SpeedModifier;
}