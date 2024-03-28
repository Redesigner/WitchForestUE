// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "WitchAbilityTask_WaitTargetData.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTABILITY_API UWitchAbilityTask_WaitTargetData : public UAbilityTask_WaitTargetData
{
	GENERATED_BODY()
	
	void Activate() override;

	void RegisterTargetDataCallbacks() override;

public:
	static UWitchAbilityTask_WaitTargetData* Witch_WaitTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, TSubclassOf<AGameplayAbilityTargetActor> Class);
};
