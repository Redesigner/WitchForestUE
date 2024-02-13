// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerState.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"

AWitchPlayerState::AWitchPlayerState()
{
    AbilitySystem = CreateDefaultSubobject<UWitchForestASC>(TEXT("WitchForestASC"));
}

UAbilitySystemComponent* AWitchPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

void AWitchPlayerState::BeginPlay()
{
    GrantAbilities();
}

void AWitchPlayerState::GrantAbilities()
{
    for (const UWitchForestAbilitySet* AbilitySet : AbilitySets)
    {
        if (!AbilitySet)
        {
            continue;
        }
        AbilitySet->GiveToAbilitySystem(AbilitySystem, nullptr);
    }
}
