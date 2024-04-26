// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "GameplayTagContainer.h"

#include "WitchForestGameState.generated.h"


UCLASS()
class WITCHFORESTGAME_API AWitchForestGameState : public AGameState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Recipes, meta = (AllowPrivateAccess = true, Categories = "RecipeTag"))
	TArray<FGameplayTag> LearnedRecipes;

public:
	TArray<FGameplayTag> GetLearnedRecipes() const { return LearnedRecipes; }

	bool HasLearnedRecipe(FGameplayTag RecipeTag) const;

	void LearnRecipe(const FGameplayTag& RecipeTag);
};
