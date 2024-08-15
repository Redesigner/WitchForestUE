// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestGameState.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "Net/UnrealNetwork.h"
#include "Logging/StructuredLog.h"

float AWitchForestGameState::GetDayRemainingSeconds() const
{
	// UE_LOGFMT(LogWitchForestGame, Display, "'{CurrentTime}' | '{EndTime}'", CurrentDayEndTimeServer, GetServerWorldTimeSeconds());
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

void AWitchForestGameState::SetCurse(UCurse* Curse)
{
	if (bCurseActive)
	{
		// Dont change the curse if there's one active right now
		return;
	}

	CurrentCurse = Curse;
}

bool AWitchForestGameState::TryLiftCurse(TArray<FGameplayTag> Items)
{
	if (!CurrentCurse->CanLiftCurse(Items))
	{
		return false;
	}

	OnCurseChanged.Broadcast(CurrentCurse, false);
	bCurseActive = false;
	return true;
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

	return 1.0f - (GetDayRemainingSeconds() / WitchForestGameMode->GetDayLength());
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
	DOREPLIFETIME_CONDITION(ThisClass, CurrentDayEndTimeServer, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, Phase, COND_SkipOwner);
}
