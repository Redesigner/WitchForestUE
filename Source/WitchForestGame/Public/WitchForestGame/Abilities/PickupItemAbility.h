// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestGame/Abilities/WitchForestGameplayAbility.h"
#include "PickupItemAbility.generated.h"

class APickup;
class UItemHandleComponent;

UCLASS()
class WITCHFORESTGAME_API UPickupItemAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()
	
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	void ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey) override;

	void ActivateAbilitySucceed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FPredictionKey& PredictionKey) override;

	
	void PickupItem(APickup* Item, UItemHandleComponent* ItemHandle);

	void SimulatePickupItem(APickup* Item, UItemHandleComponent* ItemHandle);

	void ServerPickupItem(APickup* Item, UItemHandleComponent* ItemHandle);
};
