// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestGame/Abilities/WitchForestGameplayAbility.h"
#include "RespawnAbility.generated.h"


UCLASS()
class WITCHFORESTGAME_API URespawnAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()
	
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;

public:
	URespawnAbility();
};
