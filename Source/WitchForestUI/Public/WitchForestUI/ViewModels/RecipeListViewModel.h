// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestUI/ViewModels/InventoryItemUIData.h"

#include "RecipeListViewModel.generated.h"

class AWitchForestGameState;

UCLASS(BlueprintType)
class WITCHFORESTUI_API URecipeUIData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UInventoryItemUIData>> Ingredients;

	UPROPERTY(BlueprintReadOnly)
	FInventoryItemData ResultItem;
};

UCLASS()
class WITCHFORESTUI_API URecipeListViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<URecipeUIData>> KnownRecipes;

	TWeakObjectPtr<AWitchForestGameState> OwningGameState;

	UFUNCTION(BlueprintCallable)
	void BindGameState(AWitchForestGameState* GameState);

	void RecipeLearned();
};
