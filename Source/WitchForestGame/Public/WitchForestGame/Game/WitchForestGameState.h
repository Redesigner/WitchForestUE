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

	DECLARE_MULTICAST_DELEGATE(FOnRecipeLearned);
	DECLARE_MULTICAST_DELEGATE(FOnCreatureDiscovered);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Recipes, meta = (AllowPrivateAccess = true, Categories = "RecipeTag"))
	TArray<FGameplayTag> LearnedRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bestiart, meta = (AllowPrivateAccess = true, Categories = "CreatureTag"))
	TArray<FGameplayTag> DiscoveredBestiaryEntries;

public:
	TArray<FGameplayTag> GetLearnedRecipes() const { return LearnedRecipes; }
	TArray<FGameplayTag> GetDiscoveredCreatures() const { return DiscoveredBestiaryEntries; }

	bool HasLearnedRecipe(FGameplayTag RecipeTag) const;
	void LearnRecipe(const FGameplayTag& RecipeTag);

	bool HasDiscoveredCreature(FGameplayTag CreatureTag) const;
	void DiscoverCreature(const FGameplayTag& CreatureTag);

	FOnRecipeLearned OnRecipeLearned;
	FOnCreatureDiscovered OnCreatureDiscovered;
};
