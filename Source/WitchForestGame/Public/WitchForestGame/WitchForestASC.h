// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WitchForestGame/Abilities/WitchForestAbilitySet.h"

#include "NativeGameplayTags.h"

#include "WitchForestASC.generated.h"


WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

UCLASS()
class WITCHFORESTGAME_API UWitchForestASC : public UAbilitySystemComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnAbilitiesChanged);


	TArray<FWitchForestAbilitySet_GrantedHandles> TemporaryGrantedAbilities;


	void InsertSortPriority(TArray<FGameplayAbilitySpecHandle>& Array, FGameplayAbilitySpecHandle SpecToInsert) const;

	void ClientActivateAbilityFailed_Implementation(FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey) override;
	void ClientActivateAbilitySucceedWithEventData_Implementation(FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey, FGameplayEventData TriggerEventData) override;
	void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;

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

	void RemoveAllActiveEffects();

	FOnAbilitiesChanged OnAbilitiesChanged;

protected:
	// Abilities grouped together by InputTags
	TArray<TArray<FGameplayAbilitySpecHandle>> InputPressedSpecs;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayTag> InputHeldTags;
};