// Copyright (c) 2024 Stephen Melnick

#include "WitchForestAbility/WitchForestASC.h"

#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"

UWitchForestASC::UWitchForestASC()
{   
}

void UWitchForestASC::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	bool bFoundAbilityMatchingTag = false;
	for (FGameplayAbilitySpec AbilitySpec : ActivatableAbilities.Items)
	{
		if (InputTag.MatchesAnyExact(AbilitySpec.Ability->AbilityTags))
		{
			AbilitySpec.InputPressed = true;

			if (AbilitySpec.IsActive())
			{
				AbilitySpecInputPressed(AbilitySpec);
			}
			else
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
			bFoundAbilityMatchingTag = true;
		}
	}
	if (!bFoundAbilityMatchingTag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find ability matching tag '%s'"), *InputTag.GetTagName().ToString())
	}
}

void UWitchForestASC::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec AbilitySpec : ActivatableAbilities.Items)
	{
		if (InputTag.MatchesAnyExact(AbilitySpec.Ability->AbilityTags))
		{
			AbilitySpec.InputPressed = false;

			if (AbilitySpec.IsActive())
			{
				AbilitySpecInputReleased(AbilitySpec);
			}
		}
	}
}

void UWitchForestASC::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		// This is necessary for the 'Wait Input Released' nodes to function properly
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}
