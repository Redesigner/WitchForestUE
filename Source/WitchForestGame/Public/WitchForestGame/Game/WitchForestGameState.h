// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "GameplayTagContainer.h"

#include "WitchForestGameState.generated.h"

class UCurse;
class AWitchForestGameMode;

UENUM()
enum class EWitchForestGamePhase: uint8
{
	Starting,
	Daytime,
	Intermission
};

UCLASS()
class WITCHFORESTGAME_API AWitchForestGameState : public AGameState
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnRecipeLearned);
	DECLARE_MULTICAST_DELEGATE(FOnCreatureDiscovered);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Recipes, meta = (AllowPrivateAccess = true, Categories = "RecipeTag"))
	TArray<FGameplayTag> LearnedRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bestiary, meta = (AllowPrivateAccess = true, Categories = "CreatureTag"))
	TArray<FGameplayTag> DiscoveredBestiaryEntries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curse, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurse> CurrentCurse;

public:
	UFUNCTION(BlueprintCallable)
	float GetDayRemainingSeconds() const;

	UFUNCTION(BlueprintCallable)
	uint8 GetNumberOfDaysRemaining() const;

	TArray<FGameplayTag> GetLearnedRecipes() const { return LearnedRecipes; }
	TArray<FGameplayTag> GetDiscoveredCreatures() const { return DiscoveredBestiaryEntries; }

	bool HasLearnedRecipe(FGameplayTag RecipeTag) const;
	void LearnRecipe(const FGameplayTag& RecipeTag);

	bool HasDiscoveredCreature(FGameplayTag CreatureTag) const;
	void DiscoverCreature(const FGameplayTag& CreatureTag);

	void CursePlayers();

	void SetCurse(UCurse* Curse);

	UCurse* GetCurrentCurse() const { return CurrentCurse; }

	bool TryLiftCurse(TArray<FGameplayTag> Items);

	bool IsCurseActive() const { return bCurseActive; }

	FOnRecipeLearned OnRecipeLearned;
	FOnCreatureDiscovered OnCreatureDiscovered;

	FTimerHandle CurrentDayTimer;

	UPROPERTY(Replicated)
	float CurrentDayEndTimeServer;

	UPROPERTY(Replicated)
	uint8 CurseTimeRemaining;

	EWitchForestGamePhase Phase = EWitchForestGamePhase::Starting;

private:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool bCurseActive = false;
};
