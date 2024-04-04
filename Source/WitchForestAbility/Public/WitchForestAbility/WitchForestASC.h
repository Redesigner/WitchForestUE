// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"

#include "NativeGameplayTags.h"

#include "WitchForestASC.generated.h"


WITCHFORESTABILITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

UCLASS()
class WITCHFORESTABILITY_API UWitchForestASC : public UAbilitySystemComponent
{
	GENERATED_BODY()

	TArray<FWitchForestAbilitySet_GrantedHandles> TemporaryGrantedAbilities;


	static void InsertSortPriority(TArray<FGameplayAbilitySpec>& Array, FGameplayAbilitySpec SpecToInsert);

	void ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey) override;
	void ClientActivateAbilitySucceedWithEventData_Implementation(FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData) override;

public:
	UWitchForestASC();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	void ClearAbilityInput();

	bool TryActivateAbilitiesByTag(FGameplayTag& Tag, bool bAllowRemoteActivation = true);

	// @TODO: Should this be structured differently? It breaks the dependency structure of AbilitySets as they are implemented now
	void GrantTemporaryAbilities(const UWitchForestAbilitySet* AbilitySet);

	void ClearTemporaryAbilities();

	bool GetFirstAbilityForInputTag(const FGameplayTag& InputTag, FGameplayAbilitySpec& AbilitySpecOut) const;

protected:
	// Abilities grouped together by InputTags
	TArray<TArray<FGameplayAbilitySpec>> InputPressedSpecs;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayTag> InputHeldTags;
};