// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h" 

#include "WitchForestGameMode.generated.h"

class UItemSet;
class UPotionRecipeSet;
class UBestiaryData;
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
	


	void PostLogin(APlayerController* NewPlayer) override;

public:
	AWitchForestGameMode();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UItemSet* GetItemSet() const;
	UPotionRecipeSet* GetRecipeBook() const;
	UBestiaryData* GetBestiary() const;

	void RestartIfNoLivingPlayers();
};