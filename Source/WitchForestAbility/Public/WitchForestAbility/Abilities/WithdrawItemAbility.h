// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "WithdrawItemAbility.generated.h"

class UGameplayEffect;

UCLASS()
class WITCHFORESTABILITY_API UWithdrawItemAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effect, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> ItemHeldEffect;
	
public:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
