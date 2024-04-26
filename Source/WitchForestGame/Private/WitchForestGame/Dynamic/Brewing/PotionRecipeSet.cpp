// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Brewing/PotionRecipeSet.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_RecipeFailed, "Recipe.Failure")


FGameplayTag UPotionRecipeSet::MakeItem(const TArray<FGameplayTag>& Ingredients) const
{
	for (const FPotionRecipe& Recipe : Recipes)
	{
		if (IngredientsIdentical(Ingredients, Recipe.IngredientItems))
		{
			return Recipe.ResultItem;
		}
	}

	return TAG_RecipeFailed;
}

bool UPotionRecipeSet::FindRecipeFromTag(FGameplayTag RecipeTag, FPotionRecipe& RecipeOut) const
{
	for (const FPotionRecipe& Recipe : Recipes)
	{
		if (Recipe.RecipeTag == RecipeTag)
		{
			RecipeOut = Recipe;
			return true;
		}
	}
	return false;
}

bool UPotionRecipeSet::FindRecipeFromIngredients(const TArray<FGameplayTag>& Ingredients, FPotionRecipe& RecipeOut)
{
	for (FPotionRecipe& Recipe : Recipes)
	{
		if (IngredientsIdentical(Ingredients, Recipe.IngredientItems))
		{
			RecipeOut = Recipe;
			return true;
		}
	}
	return false;
}

bool UPotionRecipeSet::IngredientsIdentical(TArray<FGameplayTag> ListA, TArray<FGameplayTag> ListB)
{
	if (ListA.Num() != ListB.Num())
	{
		return false;
	}

	ListA.Sort();
	ListB.Sort();

	for (int i = 0; i < ListA.Num(); ++i)
	{
		if (ListA[i] != ListB[i])
		{
			return false;
		}
	}
	return true;
}
