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
	
	TArray<FGameplayAbilitySpecHandle> TagHandles;
	for (FGameplayAbilitySpec AbilitySpec : ActivatableAbilities.Items)
	{
		if (InputTag.MatchesAnyExact(AbilitySpec.Ability->AbilityTags))
		{
			InsertSortPriority(TagHandles, AbilitySpec.Handle);
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
	TArray<TArray<FGameplayAbilitySpecHandle>> AbilityBucketsToActivate;

	// Process all ability groups that had their input pressed this frame.
	// i.e. abilities grouped by InputTag
	//
	for (TArray<FGameplayAbilitySpecHandle> SpecBucket : InputPressedSpecs)
	{
		TArray<FGameplayAbilitySpecHandle> NewSpecBucket;
		for (FGameplayAbilitySpecHandle& AbilitySpecHandle : SpecBucket)
		{
			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle);
			if (!AbilitySpec || !AbilitySpec->Ability)
			{
				continue;
			}

			if (AbilitySpec->IsActive())
			{
				// Ability is active so pass along the input event.
				AbilitySpec->InputPressed = true;
				AbilitySpecInputPressed(*AbilitySpec);
			}
			else
			{
				const UWitchForestGameplayAbility* AbilityCDO = CastChecked<UWitchForestGameplayAbility>(AbilitySpec->Ability);

				if (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputPressed)
				{
					// NewSpecBucket.AddUnique(AbilitySpec);
					NewSpecBucket.Add(AbilitySpecHandle);
				}
			}
		}
		AbilityBucketsToActivate.Add(NewSpecBucket);
	}


	//
	// Do the same for our buckets. If one of the abilities in the buckets succeeds,
	// break out of the loop and don't bother trying to activate any of the other abilities.
	//
	for (TArray<FGameplayAbilitySpecHandle> AbilitySpecBucket : AbilityBucketsToActivate)
	{
		for (FGameplayAbilitySpecHandle AbilitySpecHandle : AbilitySpecBucket)
		{
			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle);
			if (!AbilitySpec)
			{
				UE_LOGFMT(LogWitchForestAbility, Error, "WitchforestASC: AbilitySpec was invalid.");
				continue;
			}

			if (!AbilitySpec->Ability)
			{
				UE_LOGFMT(LogWitchForestAbility, Error, "WitchforestASC: AbilitySpec was valid, but the referenced ability pointer was invalid.");
				continue;
			}

			// Grab the ability before activating, in case the spec is removed
			UWitchForestGameplayAbility* WitchForestAbility = Cast<UWitchForestGameplayAbility>(AbilitySpec->Ability.Get());
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

void UWitchForestASC::GrantTemporaryAbilities(const UWitchForestAbilitySet* AbilitySet)
{
	if (!AbilitySet)
	{
		return;
	}

	FWitchForestAbilitySet_GrantedHandles GrantedHandles;
	AbilitySet->GiveToAbilitySystem(this, &GrantedHandles);

	TemporaryGrantedAbilities.Add(GrantedHandles);
}


void UWitchForestASC::ClearTemporaryAbilities()
{
	for (FWitchForestAbilitySet_GrantedHandles GrantedHandles : TemporaryGrantedAbilities)
	{
		GrantedHandles.TakeFromAbilitySystem(this);
	}

	TemporaryGrantedAbilities.Reset();
}


bool UWitchForestASC::GetFirstAbilityForInputTag(const FGameplayTag& InputTag, FGameplayAbilitySpec& AbilitySpecOut) const
{
	TArray<FGameplayAbilitySpecHandle> Specs;
	for (FGameplayAbilitySpec AbilitySpec : ActivatableAbilities.Items)
	{
		if (InputTag.MatchesAnyExact(AbilitySpec.Ability->AbilityTags))
		{
			InsertSortPriority(Specs, AbilitySpec.Handle);
		}
	}

	for (FGameplayAbilitySpecHandle AbilitySpecHandle : Specs)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle);

		if (!AbilitySpec)
		{
			continue;
		}

		if (UGameplayAbility* Ability = AbilitySpec->Ability)
		{
			if (Ability->CanActivateAbility(AbilitySpecHandle, AbilityActorInfo.Get()))
			{
				AbilitySpecOut = *AbilitySpec;
				return true;
			}
		}
	}
	return false;
}


void UWitchForestASC::InsertSortPriority(TArray<FGameplayAbilitySpecHandle>& Array, FGameplayAbilitySpecHandle SpecToInsert) const
{
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecToInsert);
	if (!AbilitySpec)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "WitchForestASC failed to sort ability by priority. AbilitySpecHandle was invalid.");
		return;
	}

	UWitchForestGameplayAbility* AbilityToInsert = Cast<UWitchForestGameplayAbility>(AbilitySpec->Ability);
	if (!AbilityToInsert)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "WitchForestASC failed to sort ability by priority. Ability was not of type 'WitchForestGameplayAbility'.");
		return;
	}
	uint8 InsertedAbilityPriority = AbilityToInsert->GetPriority();

	int i = 0;
	for (; i < Array.Num(); ++i)
	{
		FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromHandle(Array[i]);
		if (!ExistingSpec)
		{
			continue;
		}
		UWitchForestGameplayAbility* ExistingAbility = Cast<UWitchForestGameplayAbility>(ExistingSpec->Ability);
		if (!ExistingAbility)
		{
			continue;
		}
		if (InsertedAbilityPriority >= ExistingAbility->GetPriority())
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

void UWitchForestASC::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	OnAbilitiesChanged.Broadcast();
}
