// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "WitchForestGame/Inventory/ItemSet.h"

AWitchForestGameMode::AWitchForestGameMode()
{
}

void AWitchForestGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    
    if (!CurrentItemSet)
    {
        return;
    }
    TArray<FSoftObjectPath> ItemsToStream;
    for (FItemSetEntry& Entry : CurrentItemSet->Items)
    {
        ItemsToStream.AddUnique(Entry.ItemData.ItemIcon.ToSoftObjectPath());
    }
    StreamableManager.RequestAsyncLoad(ItemsToStream);
}

UItemSet* AWitchForestGameMode::GetItemSet() const
{
    return CurrentItemSet;
}
