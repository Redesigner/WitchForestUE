// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "ThrowItemAbility.generated.h"

class APickup;
class UItemHandleComponent;

UCLASS()
class WITCHFORESTABILITY_API UThrowItemAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	void ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey) override;

	void ActivateAbilitySucceed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FPredictionKey& PredictionKey) override;


	void ThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle);

	void SimulateThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle);

	void ServerThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle);
};
