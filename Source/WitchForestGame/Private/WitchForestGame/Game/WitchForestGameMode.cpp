// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Game/WitchForestGameInstance.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

#include "Engine/StreamableManager.h"
#include "Logging/StructuredLog.h"
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

void AWitchForestGameMode::StartPlay()
{
    Super::StartPlay();

    StartRound();
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

void AWitchForestGameMode::StartRound()
{
    AWitchForestGameState* WitchGameState = GetGameState<AWitchForestGameState>();
    if (!GameState)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartRound: WitchForestGameState is invalid.", GetName());
        return;
    }

    if (!DefaultCurse)
    {
        UE_LOGFMT(LogWitchForestGame, Warning, "'{GameModeName}' Start round failed, no curse is set.", GetName());
        return;
    }


    WitchGameState->SetCurse(NewObject<UCurse>(this, DefaultCurse.Get(), TEXT("Default Curse")));
    WitchGameState->CursePlayers();
    WitchGameState->CurseTimeRemaining = CurseTimeLimit;
    StartDay();
}

void AWitchForestGameMode::StartDay()
{
    AWitchForestGameState* WitchGameState = GetGameState<AWitchForestGameState>();
    if (!WitchGameState)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartDay: WitchForestGameState is invalid.", GetName());
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartDay: The world was invalid.", GetName());
        return;
    }

    World->GetTimerManager().SetTimer(WitchGameState->CurrentDayTimer, FTimerDelegate::CreateUObject(this, &ThisClass::EndDay), DayLengthSeconds, false, -1.0f);
}

void AWitchForestGameMode::EndDay()
{
    UE_LOGFMT(LogWitchForestGame, Display, "Day ending...");
    // Handle end of day logic
    AWitchForestGameState* WitchGameState = GetGameState<AWitchForestGameState>();
    if (!WitchGameState)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' EndDay: WitchForestGameState is invalid.", GetName());
        return;
    }

    if (WitchGameState->CurseTimeRemaining <= 1)
    {
        if (WitchGameState->IsCurseActive())
        {
            KillAllPlayers();
            // Play end sequence
            // Kill all players?
            // Restart game
        }
        else
        {
            UE_LOGFMT(LogWitchForestGame, Display, "Round ending...");
            StartRound();
        }
    }
    else
    {
        WitchGameState->CurseTimeRemaining--;
        StartDay();
    }
}

void AWitchForestGameMode::KillAllPlayers()
{
    AWitchForestGameState* WitchForestGameState = GetGameState<AWitchForestGameState>();
    if (!WitchForestGameState)
    {
        return;
    }

    for (APlayerState* PlayerState : WitchForestGameState->PlayerArray)
    {
        AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(PlayerState);
        if (!WitchPlayerState)
        {
            continue;
        }

        WitchPlayerState->GetAttributeSet()->KillOwner();
    }
}
