// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestGameState.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "Logging/StructuredLog.h"

float AWitchForestGameState::GetDayRemainingSeconds() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(CurrentDayTimer);
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

	bCurseActive = false;
	return true;
}