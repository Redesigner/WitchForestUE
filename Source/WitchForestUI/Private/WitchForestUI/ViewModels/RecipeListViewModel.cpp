// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/RecipeListViewModel.h"

#include "WitchForestGame/Dynamic/Brewing/PotionRecipeSet.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"

void URecipeListViewModel::BindGameState(AWitchForestGameState* GameState)
{
	GameState->OnRecipeLearned.AddUObject(this, &ThisClass::RecipeLearned);
	OwningGameState = GameState;

	RecipeLearned();
}

void URecipeListViewModel::RecipeLearned()
{
	if (!OwningGameState.IsValid())
	{
		return;
	}

	const AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(OwningGameState->GetDefaultGameMode());
	if (!GameMode)
	{
		return;
	}

	UPotionRecipeSet* RecipeSet = GameMode->GetRecipeBook();
	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!RecipeSet || !ItemSet)
	{
		return;
	}

	KnownRecipes.Reset();

	for (const FGameplayTag& RecipeTag : OwningGameState->GetLearnedRecipes())
	{
		FPotionRecipe Recipe;
		if (RecipeSet->FindRecipeFromTag(RecipeTag, Recipe))
		{
			URecipeUIData* RecipeData = NewObject<URecipeUIData>(this);
			FInventoryItemData InventoryItemData;
			if (ItemSet->FindItemDataForTag(Recipe.ResultItem, InventoryItemData))
			{
				RecipeData->ResultItem = InventoryItemData;
			}

			for (FGameplayTag ItemTag : Recipe.IngredientItems)
			{
				if (ItemSet->FindItemDataForTag(ItemTag, InventoryItemData))
				{
					UInventoryItemUIData* IngredientData = NewObject<UInventoryItemUIData>(this);
					IngredientData->ItemData = InventoryItemData;
					RecipeData->Ingredients.Add(IngredientData);
				}
			}
			KnownRecipes.Add(RecipeData);
		}
	}

	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::KnownRecipes);
}
