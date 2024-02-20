// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/ItemSet.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ItemEmpty, "Item.Empty")

bool UItemSet::FindItemDataForTag(const FGameplayTag& InputTag, FInventoryItemData& DataOut) const
{
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
