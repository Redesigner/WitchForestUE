// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestGameState.h"

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
