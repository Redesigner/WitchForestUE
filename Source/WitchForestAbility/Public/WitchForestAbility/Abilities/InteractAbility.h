// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "InteractAbility.generated.h"

class IInteractableInterface;

UCLASS()
class WITCHFORESTABILITY_API UInteractAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Holding, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> HeldEffectClass;


	FTimerHandle HoldTimer;

	FActiveGameplayEffectHandle HeldEffectHandle;

	TWeakInterfacePtr<IInteractableInterface> Target;

	
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void EndHoldTimer();
};
