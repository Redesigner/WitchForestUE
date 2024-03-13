// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/ItemSet.h"

#include "WitchForestGame/Dynamic/Pickup/Pickup.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_ItemEmpty, "ItemTag.Empty")

bool UItemSet::FindItemDataForTag(const FGameplayTag& InputTag, FInventoryItemData& DataOut) const
{
	if (InputTag == TAG_ItemEmpty)
	{
		return false;
	}

	for (const FItemSetEntry& Entry : Items)
	{
		if (InputTag.MatchesTagExact(Entry.ItemTag))
		{
			DataOut = Entry.ItemData;
			return true;
		}
	}
	return false;
}

bool UItemSet::FindItemTagFromClass(TSubclassOf<APickup> InputClass, FGameplayTag& TagOut) const
{
	for (const FItemSetEntry& Entry : Items)
	{
		const FInventoryItemData& Data = Entry.ItemData;
		if (Data.PickupClass == InputClass)
		{
			TagOut = Entry.ItemTag;
			return true;
		}
	}
	return false;
}
