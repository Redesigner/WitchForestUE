// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Witch/SoulSprite.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "Abilities/GameplayAbilityTypes.h" 

bool ASoulSprite::CanBeStored() const
{
	return false;
}

void ASoulSprite::OnConsume()
{
	if (!OwningASC.IsValid())
	{
		return;
	}

	FGameplayEventData Payload;
	Payload.EventTag = WitchForestGameplayTags::GameplayEvent_Respawn;
	Payload.Instigator = this;
	Payload.Target = OwningASC->GetOwnerActor();

	OwningASC->HandleGameplayEvent(Payload.EventTag, &Payload);
}

ASoulSprite::ASoulSprite()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASoulSprite::SetOwningASC(UAbilitySystemComponent* NewOwningASC)
{
	OwningASC = NewOwningASC;
}

