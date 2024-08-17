// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Curse/Curse.h"

bool FCurse::CanLiftCurse(TArray<FGameplayTag> ExistingItems) const
{
	if (ExistingItems.Num() != RequiredItems.Num())
	{
		return false;
	}

	TArray<FGameplayTag> ItemsA = ExistingItems;
	TArray<FGameplayTag> ItemsB = RequiredItems;

	ItemsA.Sort();
	ItemsB.Sort();

	return ItemsA == ItemsB;
}

void FCurse::SetRequiredItems(TArray<FGameplayTag> RequiredItemsIn)
{
	RequiredItems = RequiredItemsIn;
}
