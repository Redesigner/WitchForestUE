// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "NativeGameplayTags.h"
#include "GameplayTagContainer.h"

#include "PotionRecipeSet.generated.h"


WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_RecipeFailed);

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FPotionRecipe
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "RecipeTag"))
	FGameplayTag RecipeTag;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "ItemTag"))
	FGameplayTag ResultItem;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "ItemTag"))
	TArray<FGameplayTag> IngredientItems;
};

UCLASS()
class WITCHFORESTGAME_API UPotionRecipeSet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "RecipeTag"))
	TArray<FPotionRecipe> Recipes;

	FGameplayTag MakeItem(const TArray<FGameplayTag>& Ingredients) const;

	bool FindRecipeFromTag(FGameplayTag RecipeTag, FPotionRecipe& RecipeOut) const;

	bool FindRecipeFromIngredients(const TArray<FGameplayTag>& Ingredients, FPotionRecipe& RecipeOut);

	static bool IngredientsIdentical(TArray<FGameplayTag> ListA, TArray<FGameplayTag> ListB);
};
