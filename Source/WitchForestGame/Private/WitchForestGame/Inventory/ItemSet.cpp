// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/ItemSet.h"

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
