// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/CurseViewModel.h"

#include "Logging/StructuredLog.h"

#include "WitchForestUI.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Game/WitchForestGameState.h"

void UCurseViewModel::BindGameState(AWitchForestGameState* GameState)
{
	OwningGameState = GameState;
	GameState->OnCurseChanged.AddUObject(this, &ThisClass::CurseChanged);
	CurseChanged(GameState->GetCurrentCurse(), GameState->IsCurseActive());
}

void UCurseViewModel::CurseChanged(FCurse Curse, bool bCurseActive)
{
	CurrentlyRequiredItems.Reset();

	if (!OwningGameState.IsValid())
	{
		return;
	}

	const AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(OwningGameState->GetDefaultGameMode());
	if (!GameMode)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	for (FGameplayTag& ItemTag : Curse.RequiredItems)
	{
		FInventoryItemData InventoryItemData;
		if (ItemSet->FindItemDataForTag(ItemTag, InventoryItemData))
		{
			UInventoryItemUIData* IngredientData = NewObject<UInventoryItemUIData>(this);
			IngredientData->ItemData = InventoryItemData;
			CurrentlyRequiredItems.Add(IngredientData);
		}
	}

	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::CurrentlyRequiredItems);
}
