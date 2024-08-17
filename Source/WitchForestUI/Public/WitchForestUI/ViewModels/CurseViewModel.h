// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

// @TODO: We are including this to reuse the UInventoryItemUIData wrapper class. 
// Should this be moved out to a seprate class that both can include?
#include "WitchForestUI/ViewModels/RecipeListViewModel.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"

#include "CurseViewModel.generated.h"

class UCurse;
class AWitchForestGameState;

UCLASS()
class WITCHFORESTUI_API UCurseViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UInventoryItemUIData>> CurrentlyRequiredItems;

	TWeakObjectPtr<AWitchForestGameState> OwningGameState;
	
	UFUNCTION(BlueprintCallable)
	void BindGameState(AWitchForestGameState* GameState);

	void CurseChanged(FCurse Curse, bool bCurseActive);
};
