// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "InteractAbility.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTABILITY_API UInteractAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = true))
	FGameplayTag HeldItemAbilityTag;
	
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
