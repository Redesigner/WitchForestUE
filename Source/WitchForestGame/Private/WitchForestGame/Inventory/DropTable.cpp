// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/DropTable.h"

TArray<FDropTableAmount> UDropTable::Evaluate() const
{
    TArray<FDropTableAmount> DropTableAmounts;

    for (const FDropTableEntry& Entry : Entries)
    {
        float RandomValue = FMath::FRand();
        if (Entry.DropRate >= RandomValue)
        {
            DropTableAmounts.Add(Entry.ItemAmount);
        }
    }
    return DropTableAmounts;
}
