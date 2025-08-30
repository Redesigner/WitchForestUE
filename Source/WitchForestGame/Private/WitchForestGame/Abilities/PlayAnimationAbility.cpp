// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/PlayAnimationAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" 


void UPlayAnimationAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!Animation)
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAnimationTask"), Animation);

	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->ReadyForActivation();
}

void UPlayAnimationAbility::ForceEndAbility()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
