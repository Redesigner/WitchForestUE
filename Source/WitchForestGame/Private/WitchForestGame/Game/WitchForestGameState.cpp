// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestGameState.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


float AWitchForestGameState::GetDayRemainingSeconds() const
{
	if (Phase != EWitchForestGamePhase::Daytime)
	{
		return 0.0f;
	}

	return (CurrentDayEndTimeServer - GetServerWorldTimeSeconds());
}

uint8 AWitchForestGameState::GetNumberOfDaysRemaining() const
{
	return CurseTimeRemaining;
}

bool AWitchForestGameState::HasLearnedRecipe(FGameplayTag RecipeTag) const
{
	return LearnedRecipes.Contains(RecipeTag);
}

void AWitchForestGameState::LearnRecipe(const FGameplayTag& RecipeTag)
{
	if (LearnedRecipes.Contains(RecipeTag))
	{
		return;
	}

	LearnedRecipes.Add(RecipeTag);
	OnRecipeLearned.Broadcast();
}

bool AWitchForestGameState::HasDiscoveredCreature(FGameplayTag CreatureTag) const
{
	return DiscoveredBestiaryEntries.Contains(CreatureTag);
}

void AWitchForestGameState::DiscoverCreature(const FGameplayTag& CreatureTag)
{
	if (DiscoveredBestiaryEntries.Contains(CreatureTag))
	{
		return;
	}

	DiscoveredBestiaryEntries.Add(CreatureTag);
	OnCreatureDiscovered.Broadcast();
}

void AWitchForestGameState::CursePlayers()
{
	if (bCurseActive)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "WitchForestGameState '{Name}' failed to curse players, they are already cursed!", GetName());
		return;
	}

	OnCurseChanged.Broadcast(CurrentCurse, true);
	bCurseActive = true;
	// Actually apply curse here? Or in Gamemode?
}

void AWitchForestGameState::SetCurse(FCurse Curse)
{
	OnCurseChanged.Broadcast(CurrentCurse, bCurseActive);
	CurrentCurse = Curse;
}

bool AWitchForestGameState::TryLiftCurse(TArray<FGameplayTag> Items)
{
	if (!CurrentCurse.CanLiftCurse(Items))
	{
		return false;
	}

	LiftCurse();
	return true;
}

void AWitchForestGameState::LiftCurse()
{
	OnCurseChanged.Broadcast(CurrentCurse, false);
	bCurseActive = false;

	FWitchForestMessage NewMessage;
	NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
	NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Lifted;
	BroadcastMessageAllClients(NewMessage);
}

float AWitchForestGameState::GetDayTime() const
{
	if (Phase != EWitchForestGamePhase::Daytime)
	{
		return -1.0f;
	}

	const AWitchForestGameMode* WitchForestGameMode = Cast<AWitchForestGameMode>(GetDefaultGameMode());
	if (!WitchForestGameMode)
	{
		return -1.0f;
	}

	// Rework this so we don't need to call the game mode here
	return 1.0f - (GetDayRemainingSeconds() / WitchForestGameMode->GetDayLength());
}

void AWitchForestGameState::BroadcastMessageAllClients(const FWitchForestMessage& Message)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(PlayerState);
		if (!WitchPlayerState)
		{
			continue;
		}

		WitchPlayerState->ClientBroadcastMessage(Message);
	}

	// Broadcast the message to the host, in case this is a listen server
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}

void AWitchForestGameState::MulticastStartDay_Implementation()
{
	OnDayStart.Broadcast();
}

void AWitchForestGameState::MulticastEndDay_Implementation()
{
	OnDayEnd.Broadcast();
}

void AWitchForestGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurseTimeRemaining);
	DOREPLIFETIME(ThisClass, CurrentCurse);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentDayEndTimeServer, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, Phase, COND_SkipOwner);
}
