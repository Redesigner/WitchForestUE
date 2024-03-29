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
