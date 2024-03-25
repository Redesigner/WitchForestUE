// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellReticleAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "Logging/StructuredLog.h"

#include "WitchForestAbility.h"

void USpellReticleAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilityTask_WaitTargetData* WaitTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetData", EGameplayTargetingConfirmation::Instant, ReticleClass);
	WaitTargetTask->ReadyForActivation();

	UE_LOGFMT(LogWitchForestAbility, Display, "SpellReticleAbility '{SelfName}' activated.", GetName());
}
