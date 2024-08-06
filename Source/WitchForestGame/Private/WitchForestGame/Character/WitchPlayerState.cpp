// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerState.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestGame.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "Net/UnrealNetwork.h"
#include "Logging/StructuredLog.h"


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

void AWitchPlayerState::OverrideTeam(EWitchForestTeam NewTeam)
{
    // Can't override the player's team for now
}

void AWitchPlayerState::SetWitchForestTeam(EWitchForestTeam InTeam)
{
    TeamId = InTeam;
}

EWitchForestTeam AWitchPlayerState::GetWitchForestTeam() const
{
    return TeamId;
}


UBaseAttributeSet* AWitchPlayerState::GetAttributeSet() const
{
    return AttributeSet;
}


void AWitchPlayerState::BeginPlay()
{
}


void AWitchPlayerState::GrantAbilities()
{
    if (bAbilitiesGranted)
    {
        // Don't grant default abilities again
        return;
    }

    for (const UWitchForestAbilitySet* AbilitySet : AbilitySets)
    {
        if (!AbilitySet)
        {
            continue;
        }
        UE_LOGFMT(LogWitchForestGame, Display, "AbilitySet '{AbilitySetName}' granted to player state '{PlayerStateName}'", AbilitySet->GetName(), GetName());
        AbilitySet->GiveToAbilitySystem(AbilitySystem, nullptr);
    }
    bAbilitiesGranted = true;
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
