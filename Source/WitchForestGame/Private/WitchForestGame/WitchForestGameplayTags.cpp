// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/WitchForestGameplayTags.h"

namespace WitchForestGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Respawn, "GameplayEvent.Respawn");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_AbilityTrigger_Throw, "GameplayEvent.AbilityTrigger.Throw");

	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");

	UE_DEFINE_GAMEPLAY_TAG(Element_Fire, "Element.Fire");

	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Stun, "GameplayEffect.Stun");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Immobile, "GameplayEffect.Immobile");
}