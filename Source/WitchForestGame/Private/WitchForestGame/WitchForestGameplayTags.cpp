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
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Duration, "SetByCaller.Duration");

	UE_DEFINE_GAMEPLAY_TAG(Element_Fire, "Element.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Element_Ice, "Element.Ice");
	UE_DEFINE_GAMEPLAY_TAG(Element_Plant, "Element.Plant");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffectData, "GameplayEffectData", "Tag representing data for gameplayeffect, such as icon, name, etc.");

	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Stun, "GameplayEffect.Stun");
	UE_DEFINE_GAMEPLAY_TAG(GameplayEffect_Immobile, "GameplayEffect.Immobile");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Danger, "GameplayEffect.Danger", "In an area where respawn (such as from falling) is not valid.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Blind, "GameplayEffect.Blind", "Blocks enemy sight perception. Does not currently impact players.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Sight, "GameplayEffect.Sight", "Counters blind stacks. Does not currently impact players.");

	UE_DEFINE_GAMEPLAY_TAG(Event_Discovery, "Event.Discovery");
	UE_DEFINE_GAMEPLAY_TAG(Event_Discovery_Recipe, "Event.Discovery.Recipe");
	UE_DEFINE_GAMEPLAY_TAG(Event_Notification, "Event.Notification");

	// Gameplay tags for our natively defined notifications, to be used with our UI
	UE_DEFINE_GAMEPLAY_TAG(Notification_Curse, "Notification.Curse");
	UE_DEFINE_GAMEPLAY_TAG(Notification_Curse_Afflicted, "Notification.Curse.Afflicted");
	UE_DEFINE_GAMEPLAY_TAG(Notification_Curse_Lifted, "Notification.Curse.Lifted");
	UE_DEFINE_GAMEPLAY_TAG(Notification_Curse_Lethal, "Notification.Curse.Lethal");

#if !UE_BUILD_SHIPPING
	UE_DEFINE_GAMEPLAY_TAG(Cheat_Immortal, "Cheat.Immortal");
#endif // !UE_BUILD_SHIPPING
}