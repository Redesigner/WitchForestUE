// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/Layout/WitchForestLayoutMap.h"

bool UWitchForestLayoutMap::FindWidgetLayoutForMessage(FGameplayTag MessageTag, FWitchForestLayoutMapEntry& WidgetLayoutOut) const
{
	for (const FWitchForestLayoutMapEntry& Entry : LayoutEntries)
	{
		if (Entry.MessageTag == MessageTag)
		{
			WidgetLayoutOut = Entry;
			return true;
		}
	}

	return false;
}
