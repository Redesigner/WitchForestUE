// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/ItemContainerViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "WitchForestGame/Dynamic/Interactable/ItemContainer.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Game/WitchForestGameState.h"

void UItemContainerViewModel::BindItemContainer(AItemContainer* ItemContainer)
{
	if (!ItemContainer)
	{
		return;
	}

	BoundContainer = ItemContainer;
	ItemContainer->OnContentsChanged.AddUObject(this, &ThisClass::ContentsChanged);
	ContentsChanged(ItemContainer->GetItems()); // Force update, otherwise it won't reflect the current state until the contents change again
}

void UItemContainerViewModel::RemoveItem(APlayerController* Requester)
{
	if (!BoundContainer.IsValid())
	{
		return;
	}

	BoundContainer->RequestLaunchItemByIndex(Requester, SelectedIndex);
}

void UItemContainerViewModel::ContentsChanged(const TArray<FGameplayTag>& Contents)
{
	AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
	if (!GameState)
	{
		return;
	}

	// Get the *default* gamemode by way of the game state, rather than the gamemode directly
	// the gamemode from GameplayStatics does not exist on clients
	const AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(GameState->GetDefaultGameMode());
	if (!GameMode)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	ContainerItems.Empty();

	for (const FGameplayTag& ItemTag : Contents)
	{
		FInventoryItemData InventoryItemData;
		if (ItemSet->FindItemDataForTag(ItemTag, InventoryItemData))
		{
			UInventoryItemUIData* ContentData = NewObject<UInventoryItemUIData>(this);
			ContentData->ItemData = InventoryItemData;
			ContainerItems.Add(ContentData);
		}
		else if (ItemTag == FGameplayTag::EmptyTag)
		{
			// Special case for empty item?
			UInventoryItemUIData* ContentData = NewObject<UInventoryItemUIData>(this);
			ContentData->ItemData = FInventoryItemData();
			ContainerItems.Add(ContentData);
		}
	}

	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::ContainerItems);
}
