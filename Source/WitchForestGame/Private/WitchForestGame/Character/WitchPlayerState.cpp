// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerState.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "Net/UnrealNetwork.h"


AWitchPlayerState::AWitchPlayerState()
{
    AbilitySystem = CreateDefaultSubobject<UWitchForestASC>(TEXT("WitchForestASC"));

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

    AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));
    AttributeSet->OnDeath.AddUObject(this, &ThisClass::OnAttributeSetDeath);
}


UAbilitySystemComponent* AWitchPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

FGenericTeamId AWitchPlayerState::GetGenericTeamId() const
{
    return FGenericTeamId();
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

void AWitchPlayerState::InitializeAttributes()
{
    AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
    bAlive = true;
}

void AWitchPlayerState::OnAttributeSetDeath(FGameplayEffectSpec SpecCauser)
{
    bAlive = false;
    OnDeath.Broadcast();
}
