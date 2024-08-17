// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "GameplayTagContainer.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"

#include "WitchForestGameState.generated.h"

class AWitchForestGameMode;

UENUM()
enum class EWitchForestGamePhase: uint8
{
	Starting,
	Daytime,
	Nighttime,
	Intermission
};

UCLASS()
class WITCHFORESTGAME_API AWitchForestGameState : public AGameState
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnRecipeLearned);
	DECLARE_MULTICAST_DELEGATE(FOnCreatureDiscovered);
	DECLARE_MULTICAST_DELEGATE(FOnDayEnd);
	DECLARE_MULTICAST_DELEGATE(FOnDayStart);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurseChanged, FCurse, bool);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Recipes, meta = (AllowPrivateAccess = true, Categories = "RecipeTag"))
	TArray<FGameplayTag> LearnedRecipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bestiary, meta = (AllowPrivateAccess = true, Categories = "CreatureTag"))
	TArray<FGameplayTag> DiscoveredBestiaryEntries;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Curse, meta = (AllowPrivateAccess = true))
	FCurse CurrentCurse;

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

	void SetCurse(FCurse Curse);

	FCurse GetCurrentCurse() const { return CurrentCurse; }

	bool TryLiftCurse(TArray<FGameplayTag> Items);

	bool IsCurseActive() const { return bCurseActive; }

	float GetDayTime() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEndDay();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartDay();


	FOnRecipeLearned OnRecipeLearned;
	FOnCreatureDiscovered OnCreatureDiscovered;

	FOnDayEnd OnDayEnd;
	FOnDayStart OnDayStart;

	FOnCurseChanged OnCurseChanged;

	FTimerHandle CurrentDayTimer;

	UPROPERTY(Replicated)
	float CurrentDayEndTimeServer;

	UPROPERTY(Replicated)
	uint8 CurseTimeRemaining;

	UPROPERTY(Replicated)
	EWitchForestGamePhase Phase = EWitchForestGamePhase::Starting;

private:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool bCurseActive = false;
};
