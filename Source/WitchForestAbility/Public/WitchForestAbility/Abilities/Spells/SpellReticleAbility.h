// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "SpellReticleAbility.generated.h"


class AGameplayAbilityTargetActor_GroundTrace;

UCLASS()
class WITCHFORESTABILITY_API USpellReticleAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Targeting, meta = (AllowPrivateAccess = true))
	TSubclassOf<AGameplayAbilityTargetActor_GroundTrace> ReticleClass;
	
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;
};
