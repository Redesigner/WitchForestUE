// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestGameState.h"

bool AWitchForestGameState::HasLearnedRecipe(FGameplayTag RecipeTag) const
{
	return LearnedRecipes.Contains(RecipeTag);
}

void AWitchForestGameState::LearnRecipe(const FGameplayTag& RecipeTag)
{
	LearnedRecipes.AddUnique(RecipeTag);
}
