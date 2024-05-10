// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/DropTable.h"

TArray<FDropTableAmount> UDropTable::Evaluate(float Power) const
{
    TArray<FDropTableAmount> DropTableAmounts;

    for (const FDropTableEntry& Entry : Entries)
    {
        float RandomValue = FMath::FRand();
        if (Entry.DropRate >= RandomValue * (Power / 100.0f))
        {
            DropTableAmounts.Add(Entry.ItemAmount);
        }
    }
    return DropTableAmounts;
}
