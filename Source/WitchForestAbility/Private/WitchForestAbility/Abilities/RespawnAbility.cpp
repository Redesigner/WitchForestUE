// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/RespawnAbility.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/WitchPlayerController.h"
#include "WitchForestAbility.h"

#include "Logging/StructuredLog.h"

void URespawnAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo->PlayerController.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		UE_LOGFMT(LogWitchForestAbility, Error, "RespawnAbility '{AbilityName}' failed to respawn. PlayerController was invalid.", GetName());
		return;
	}

	UE_LOGFMT(LogWitchForestAbility, Display, "RespawnAbility '{AbilityName}' respawning player.", GetName());
	ActorInfo->PlayerController->ServerRestartPlayer_Implementation();
}

URespawnAbility::URespawnAbility()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = WitchForestGameplayTags::GameplayEvent_Respawn;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}
