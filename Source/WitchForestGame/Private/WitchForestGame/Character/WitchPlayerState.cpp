// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerState.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"


AWitchPlayerState::AWitchPlayerState()
{
    AbilitySystem = CreateDefaultSubobject<UWitchForestASC>(TEXT("WitchForestASC"));

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

    AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));
}


UAbilitySystemComponent* AWitchPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

UBaseAttributeSet* AWitchPlayerState::GetAttributeSet() const
{
    return AttributeSet;
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
