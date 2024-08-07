// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Game/WitchForestGameInstance.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "Engine/StreamableManager.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameplayMessageSubsystem.h"


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
        // WitchPlayerState->OnDeath.AddUObject(this, &ThisClass::RestartIfNoLivingPlayers);
        WitchPlayerState->OnDeath.AddUObject(this, &ThisClass::EndDayIfNoLivingPlayers);
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
    if (!AnyPlayersAlive())
    {
        FTimerHandle RestartTimer;
        GetWorld()->GetTimerManager().SetTimer(RestartTimer,  FTimerDelegate::CreateLambda([this]()
            {
                RestartGame();
            }),
            RestartTime, false, -1.0f);
    }
}

void AWitchForestGameMode::EndDayIfNoLivingPlayers()
{
    if (AWitchForestGameState* WitchGameState = GetGameState<AWitchForestGameState>())
    {
        if (WitchGameState->CurseTimeRemaining <= 0)
        {
            return;
        }
    }

    if (!AnyPlayersAlive())
    {
        UE_LOGFMT(LogWitchForestGame, Display, "WitchForestGameMode: Day ending because all players have died.");
        EndDay();
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

    UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
    FWitchForestMessage NewMessage;
    NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
    NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Afflicted;
    NewMessage.Source = this;
    MessageSystem.BroadcastMessage(NewMessage.Verb, NewMessage);
}

void AWitchForestGameMode::StartDay()
{
    AWitchForestGameState* WitchGameState = GetGameState<AWitchForestGameState>();
    if (!WitchGameState)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartDay: WitchForestGameState is invalid.", GetName());
        return;
    }

    if (WitchGameState->Phase == EWitchForestGamePhase::Daytime)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartDay failed: the day has already started.", GetName());
        return;
    }

    WitchGameState->Phase = EWitchForestGamePhase::Daytime;

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartDay: The world was invalid.", GetName());
        return;
    }

    UE_LOGFMT(LogWitchForestGame, Display, "'{GameModeName}' Day started. {DayCount} days remain.", GetName(), WitchGameState->CurseTimeRemaining);

    RespawnDeadPlayers();
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

    if (WitchGameState->Phase != EWitchForestGamePhase::Daytime)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' EndDay failed. Day has already ended. Make sure to call StartDay.", GetName());
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(WitchGameState->CurrentDayTimer);
    WitchGameState->Phase = EWitchForestGamePhase::Intermission;

    if (bKillUnsafePlayersAtNight)
    {
        KillUnsafePlayers();
    }

    if (WitchGameState->CurseTimeRemaining <= 1)
    {
        if (WitchGameState->IsCurseActive())
        {
            WitchGameState->CurseTimeRemaining--;
            EndGame();
        }
        else
        {
            UE_LOGFMT(LogWitchForestGame, Display, "Round ending...");

            FTimerHandle IntermissionTimer;
            GetWorld()->GetTimerManager().SetTimer(IntermissionTimer, FTimerDelegate::CreateUObject(this, &ThisClass::StartRound), IntermissionTime, false, -1.0f);
        }
    }
    else
    {
        WitchGameState->CurseTimeRemaining--;
        FTimerHandle IntermissionTimer;
        GetWorld()->GetTimerManager().SetTimer(IntermissionTimer, FTimerDelegate::CreateUObject(this, &ThisClass::StartDay), IntermissionTime, false, -1.0f);
    }
}

void AWitchForestGameMode::EndGame()
{
    KillAllPlayers();
    UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
    FWitchForestMessage NewMessage;
    NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
    NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Lethal;
    NewMessage.Source = this;
    MessageSystem.BroadcastMessage(NewMessage.Verb, NewMessage);

    FTimerHandle RestartGameTimer;
    GetWorld()->GetTimerManager().SetTimer(RestartGameTimer, FTimerDelegate::CreateLambda([this]()
        {
            RestartGame();
        }),
        RestartTime, false, -1.0f);
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

void AWitchForestGameMode::KillUnsafePlayers()
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

        UWitchForestASC* ASC = WitchPlayerState->GetWitchForestASC();
        if (!ASC)
        {
            continue;
        }

        if (WitchPlayerState->IsAlive() && ASC->HasMatchingGameplayTag(WitchForestGameplayTags::GameplayEffect_Outside))
        {
            WitchPlayerState->GetAttributeSet()->KillOwner();
        }
    }
}

void AWitchForestGameMode::RespawnDeadPlayers()
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

        if (!WitchPlayerState->IsAlive() && WitchPlayerState->GetWitchForestASC())
        {
            FGameplayEventData Payload;
            Payload.EventTag = WitchForestGameplayTags::GameplayEvent_Respawn;
            Payload.Instigator = this;
            Payload.Target = WitchPlayerState->GetWitchForestASC()->GetOwnerActor();

            WitchPlayerState->GetWitchForestASC()->HandleGameplayEvent(Payload.EventTag, &Payload);
        }
    }
}

bool AWitchForestGameMode::AnyPlayersAlive() const
{
    AWitchForestGameState* WitchForestGameState = GetGameState<AWitchForestGameState>();
    if (!WitchForestGameState)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "WitchForestGameMode cannot determine if any players are alive, the game state was invalid.");
        return false;
    }

    for (APlayerState* PlayerState : WitchForestGameState->PlayerArray)
    {
        AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(PlayerState);
        if (!WitchPlayerState)
        {
            continue;
        }

        if (WitchPlayerState->IsAlive())
        {
            return true;
        }
    }
    return false;
}
