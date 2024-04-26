// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h" 

#include "WitchForestGameMode.generated.h"

class UItemSet;
class UPotionRecipeSet;

UCLASS()
class WITCHFORESTGAME_API AWitchForestGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	TObjectPtr<UItemSet> CurrentItemSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Recipes, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPotionRecipeSet> RecipeBook;


	void PostLogin(APlayerController* NewPlayer) override;

public:
	AWitchForestGameMode();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UItemSet* GetItemSet() const;

	UPotionRecipeSet* GetRecipeBook() const;

	void RestartIfNoLivingPlayers();
};