// Copyright (c) 2024 Stephen Melnick

#include "WitchForestAbility/WitchForestASC.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"

#include "Logging/StructuredLog.h"

UWitchForestASC::UWitchForestASC()
{   
}

void UWitchForestASC::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	if (InputHeldTags.Contains(InputTag))
	{
		return;
	}

	InputHeldTags.Add(InputTag);
	
	TArray<FGameplayAbilitySpec> TagHandles;
	for (FGameplayAbilitySpec AbilitySpec : ActivatableAbilities.Items)
	{
		if (InputTag.MatchesAnyExact(AbilitySpec.Ability->AbilityTags))
		{
			InsertSortPriority(TagHandles, AbilitySpec);
		}
	}

	// Sort the specs by priority before adding them to the 'stack'
	if (TagHandles.Num() > 0)
	{
		InputPressedSpecs.Add(TagHandles);
		UE_LOGFMT(LogWitchForestAbility, Verbose, "WitchForestASC: Found '{Num}' Ability Specs matching InputTag '{Tag}'.", TagHandles.Num(), InputTag.GetTagName());
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
			if (AbilitySpec.IsActive())
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}

	InputHeldTags.Remove(InputTag);
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

void UWitchForestASC::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray <TArray<FGameplayAbilitySpec*>> AbilityBucketsToActivate;
	AbilityBucketsToActivate.Reset();


	// Process all ability groups that had their input pressed this frame.
	// i.e. abilities grouped by InputTag
	//
	for (TArray<FGameplayAbilitySpec> SpecBucket : InputPressedSpecs)
	{
		TArray<FGameplayAbilitySpec*> NewSpecBucket;
		for (FGameplayAbilitySpec& AbilitySpec : SpecBucket)
		{
			if (!AbilitySpec.Ability)
			{
				continue;
			}

			if (AbilitySpec.IsActive())
			{
				// Ability is active so pass along the input event.
				AbilitySpec.InputPressed = true;
				AbilitySpecInputPressed(AbilitySpec);
			}
			else
			{
				const UWitchForestGameplayAbility* AbilityCDO = CastChecked<UWitchForestGameplayAbility>(AbilitySpec.Ability);

				if (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputPressed)
				{
					NewSpecBucket.AddUnique(&AbilitySpec);
				}
			}
		}
		AbilityBucketsToActivate.Add(NewSpecBucket);
	}


	//
	// Do the same for our buckets. If one of the abilities in the buckets succeeds,
	// break out of the loop and don't bother trying to activate any of the other abilities.
	//
	for (TArray<FGameplayAbilitySpec*> AbilitySpecBucket : AbilityBucketsToActivate)
	{
		for (FGameplayAbilitySpec* AbilitySpec : AbilitySpecBucket)
		{
			// Grab the ability before activating, in case the spec is removed
			UWitchForestGameplayAbility* WitchForestAbility = Cast<UWitchForestGameplayAbility>(AbilitySpec->Ability);
			if (WitchForestAbility && AbilitySpec->InputPressed && WitchForestAbility->ConsumesInput())
			{
				UE_LOGFMT(LogWitchForestAbility, Verbose, "WitchForestASC: Ability {AbilityName}, currently being held, consumed input.", WitchForestAbility->GetName());
				break;
			}
			if (TryActivateAbility(AbilitySpec->Handle))
			{
				if (WitchForestAbility && WitchForestAbility->ConsumesInput())
				{
					UE_LOGFMT(LogWitchForestAbility, Verbose, "WitchForestASC: Ability {AbilityName} consumed input.", WitchForestAbility->GetName());
					break;
				}
			}
		}
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecs.Reset();
	InputReleasedSpecHandles.Reset();
}

void UWitchForestASC::ClearAbilityInput()
{
}

bool UWitchForestASC::TryActivateAbilitiesByTag(FGameplayTag& Tag, bool bAllowRemoteActivation)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (Tag.MatchesAnyExact(AbilitySpec.Ability->AbilityTags))
		{
			AbilitiesToActivate.AddUnique(AbilitySpec.Handle);
		}
	}
	bool bSuccess = false;
	
	for (FGameplayAbilitySpecHandle AbilitySpec : AbilitiesToActivate)
	{
		bSuccess |= TryActivateAbility(AbilitySpec, bAllowRemoteActivation);
	}

	return bSuccess;
}

void UWitchForestASC::InsertSortPriority(TArray<FGameplayAbilitySpec>& Array, FGameplayAbilitySpec SpecToInsert)
{
	UWitchForestGameplayAbility* AbilityToInsert = Cast<UWitchForestGameplayAbility>(SpecToInsert.Ability);
	if (!AbilityToInsert)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "WitchForestASC failed to sort ability by priority. Ability was not of type 'WitchForestGameplayAbility'.");
		return;
	}
	uint8 InsertedAbilityPriority = AbilityToInsert->GetPriority();

	int i = 0;
	for (; i < Array.Num(); ++i)
	{
		UWitchForestGameplayAbility* ExisistingAbility = Cast<UWitchForestGameplayAbility>(Array[i].Ability);
		if (InsertedAbilityPriority >= ExisistingAbility->GetPriority())
		{
			break;
		}
	}

	Array.Insert(SpecToInsert, i);
}

void UWitchForestASC::ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey)
{
	Super::ClientActivateAbilityFailed_Implementation(AbilityToActivate, PredictionKey);

	FGameplayAbilitySpec* FailedAbilitySpec = FindAbilitySpecFromHandle(AbilityToActivate);
	if (!FailedAbilitySpec)
	{
		return;
	}
	
	if (UWitchForestGameplayAbility* FailedAbility = Cast<UWitchForestGameplayAbility>(FailedAbilitySpec->Ability))
	{
		const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
		FailedAbility->ActivateAbilityFailed(AbilityToActivate, ActorInfo, FailedAbilitySpec->ActivationInfo, PredictionKey);
	}
}

void UWitchForestASC::ClientActivateAbilitySucceedWithEventData_Implementation(FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData)
{
	Super::ClientActivateAbilitySucceedWithEventData_Implementation(Handle, PredictionKey, TriggerEventData);

	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(Handle);
	if (!AbilitySpec)
	{
		return;
	}

	if (UWitchForestGameplayAbility* FailedAbility = Cast<UWitchForestGameplayAbility>(AbilitySpec->Ability))
	{
		const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
		FailedAbility->ActivateAbilitySucceed(Handle, ActorInfo, AbilitySpec->ActivationInfo, PredictionKey);
	}
}
