// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Bestiary/BestiaryData.h"

bool UBestiaryData::FindBestiaryForTag(const FGameplayTag& InputTag, FBestiaryEntry& DataOut) const
{
	for (const FBestiaryEntry& BestiaryEntry : Entries)
	{
		if (BestiaryEntry.CreatureTag == InputTag)
		{
			DataOut = BestiaryEntry;
			return true;
		}
	}

	return false;
}
