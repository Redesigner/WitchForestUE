// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "PickupItemAbility.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTABILITY_API UPickupItemAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()
	
public:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
