// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/ItemContainerViewModel.h"

#include "Kismet/GameplayStatics.h"

#include "WitchForestGame/Dynamic/Interactable/ItemContainer.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"

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

void UItemContainerViewModel::RemoveItem()
{
	if (!BoundContainer.IsValid())
	{
		return;
	}

	BoundContainer->LaunchItemByIndex(SelectedIndex);
}

void UItemContainerViewModel::ContentsChanged(const TArray<FGameplayTag>& Contents)
{
	const AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
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
