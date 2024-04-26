// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/WitchForestGameplayTags.h"

namespace WitchForestGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look, "InputTag.Look", "Look input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_ShiftSlot, "InputTag.ShiftSlot", "Inventory slot change input.");

	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Death, "GameplayEvent.Death");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_Respawn, "GameplayEvent.Respawn");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEvent_AbilityTrigger_Throw, "GameplayEvent.AbilityTrigger.Throw");

	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");

	UE_DEFINE_GAMEPLAY_TAG(Element_Fire, "Element.Fire");

	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Stun, "GameplayEffect.Stun");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Immobile, "GameplayEffect.Immobile");

	UE_DEFINE_GAMEPLAY_TAG(Event_Discovery, "Event.Discovery");
	UE_DEFINE_GAMEPLAY_TAG(Event_Discovery_Recipe, "Event.Discovery.Recipe");
}