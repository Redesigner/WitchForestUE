// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/DropTable.h"

#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"

TArray<FDropTableAmount> UDropTable::Evaluate(float Power) const
{
    TArray<FDropTableAmount> DropTableAmounts;

    for (const FDropTableEntry& Entry : Entries)
    {
        float RandomValue = FMath::FRand();
        // UE_LOGFMT(LogWitchForestGame, Display, "Evaluating drop table entry for '{EntryName}' with a drop rate of '{DropRate}'. Power multiplier '{Multiplier}' - rolled a '{RandomValue}'", Entry.ItemAmount.ItemTag.ToString(), Entry.DropRate, Power / 100.0f, RandomValue);
        if (Entry.DropRate * (Power / 100.0f) >= RandomValue)
        {
            // UE_LOGFMT(LogWitchForestGame, Display, "Dropping item.");
            DropTableAmounts.Add(Entry.ItemAmount);
        }
    }
    return DropTableAmounts;
}
