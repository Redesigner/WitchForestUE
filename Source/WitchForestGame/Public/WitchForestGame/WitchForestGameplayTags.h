// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "NativeGameplayTags.h"

namespace WitchForestGameplayTags
{
	WITCHFORESTGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	WITCHFORESTGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	WITCHFORESTGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ShiftSlot);

	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Respawn);
	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_AbilityTrigger_Throw);

	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);

	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Element_Fire);

	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Stun);
	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Immobile);

	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Discovery);
	WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Discovery_Recipe);
}