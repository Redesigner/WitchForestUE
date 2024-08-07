// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h" 

#include "WitchForestGameMode.generated.h"

class UItemSet;
class UPotionRecipeSet;
class UBestiaryData;
class UCurse;
class UGameplayEffectDataSet;

UCLASS()
class WITCHFORESTGAME_API AWitchForestGameMode : public AGameMode
{
	GENERATED_BODY()

	// These sets should only be changed as defaults. We can only get defaults when in multiplayer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	TObjectPtr<UItemSet> CurrentItemSet;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Recipes, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPotionRecipeSet> RecipeBook;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Bestiary, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBestiaryData> Bestiary;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Restart, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float RestartTime = 5.0f;

	// Should players outside of a safe zone be killed when night falls?
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Time, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	bool bKillUnsafePlayersAtNight = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = Time, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float IntermissionTime = 5.0f;

	// How long until the curse becomes lethal, in days
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curse, meta = (AllowPrivateAccess = true))
	uint8 CurseTimeLimit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curse, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float DayLengthSeconds = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Curse, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	TSubclassOf<UCurse> DefaultCurse;
	
public:
	AWitchForestGameMode();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UItemSet* GetItemSet() const;
	UPotionRecipeSet* GetRecipeBook() const;
	UBestiaryData* GetBestiary() const;

	void RestartIfNoLivingPlayers();
	void EndDayIfNoLivingPlayers();

	uint8 GetCurseTimeLimit() const { return CurseTimeLimit; }
	float GetDayLength() const { return DayLengthSeconds; }

private:
	void PostLogin(APlayerController* NewPlayer) override;

	void StartPlay() override;

	void StartRound();
	void StartDay();
	void EndDay();
	void EndGame();

	void KillAllPlayers();
	void KillUnsafePlayers();
	void RespawnDeadPlayers();

	bool AnyPlayersAlive() const;
};