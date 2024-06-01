// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Game/WitchForestGameInstance.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"


AWitchForestGameMode::AWitchForestGameMode()
{
}

void AWitchForestGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (!NewPlayer)
    {
        return;
    }

    if (AWitchPlayerState* WitchPlayerState = NewPlayer->GetPlayerState<AWitchPlayerState>())
    {
        WitchPlayerState->OnDeath.AddUObject(this, &ThisClass::RestartIfNoLivingPlayers);
    }
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

UPotionRecipeSet* AWitchForestGameMode::GetRecipeBook() const
{
    return RecipeBook;
}

UBestiaryData* AWitchForestGameMode::GetBestiary() const
{
    return Bestiary;
}

void AWitchForestGameMode::RestartIfNoLivingPlayers()
{
    AWitchForestGameState* WitchForestGameState = GetGameState<AWitchForestGameState>();
    if (!WitchForestGameState)
    {
        return;
    }

    bool bPlayersAlive = false;

    for (APlayerState* PlayerState : WitchForestGameState->PlayerArray)
    {
        AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(PlayerState);
        if (!WitchPlayerState)
        {
            continue;
        }

        if (WitchPlayerState->IsAlive())
        {
            bPlayersAlive = true;
        }
    }

    if (!bPlayersAlive)
    {
        FTimerHandle RestartTimer;
        GetWorld()->GetTimerManager().SetTimer(RestartTimer,  FTimerDelegate::CreateLambda([this]()
            {
                RestartGame();
            }),
            RestartTime, false, -1.0f);
    }
}
