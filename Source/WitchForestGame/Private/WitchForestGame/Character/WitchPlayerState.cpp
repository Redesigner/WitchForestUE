// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerState.h"

#include "WitchForestGame/WitchForestASC.h"
#include "WitchForestGame/Abilities/WitchForestAbilitySet.h"
#include "WitchForestGame/Attributes/BaseAttributeSet.h"
#include "WitchForestGame.h"
#include "WitchForestGame/Character/WitchPlayerController.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"
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

void AWitchPlayerState::ClientBroadcastMessage_Implementation(const FWitchForestMessage Message)
{
    // This is how Lyra handles broadcasting messages to clients!
    if (GetNetMode() != NM_Client)
    {
        return;
    }

    if (bReceivingNotifications)
    {
        UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
    }
    else
    {
        UnprocessedNotifications.Enqueue(Message);
    }
}


void AWitchPlayerState::BeginPlay()
{
    if (AWitchPlayerController* WitchPlayerController = Cast<AWitchPlayerController>(GetPlayerController()))
    {
        WitchPlayerController->OnHUDCreated.AddUObject(this, &ThisClass::FlushNotifications);
    }
}

void AWitchPlayerState::FlushNotifications()
{
    FWitchForestMessage Notification;
    while (UnprocessedNotifications.Dequeue(Notification))
    {
        UGameplayMessageSubsystem::Get(this).BroadcastMessage(Notification.Verb, Notification);
    }
    bReceivingNotifications = true;
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
    
    AbilitySystem->RemoveAllActiveEffects();
    OnDeath.Broadcast();
}
