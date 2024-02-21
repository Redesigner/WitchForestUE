// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "WitchForestGame/Game/WitchForestGameInstance.h"
#include "WitchForestGame/Inventory/ItemSet.h"

#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

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
    UWitchForestGameInstance* GameInstance = Cast<UWitchForestGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        return;
    }
    TArray<FSoftObjectPath> ItemsToStream;
    for (FItemSetEntry& Entry : CurrentItemSet->Items)
    {
        ItemsToStream.AddUnique(Entry.ItemData.ItemIcon.ToSoftObjectPath());
    }
    GameInstance->GetStreamableManager().RequestAsyncLoad(ItemsToStream);
}

UItemSet* AWitchForestGameMode::GetItemSet() const
{
    return CurrentItemSet;
}
