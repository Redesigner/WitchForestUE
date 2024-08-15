// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Character/WitchPlayerController.h"
#include "WitchForestGame/Dynamic/Curse/Curse.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Game/WitchForestGameInstance.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "Engine/StreamableManager.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"


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

    AWitchPlayerState* WitchPlayerState = NewPlayer->GetPlayerState<AWitchPlayerState>();
    if (!WitchPlayerState)
    {
        return;
    }

    AWitchPlayerController* WitchPlayerController = Cast<AWitchPlayerController>(NewPlayer);
    if (!WitchPlayerController)
    {
        return;
    }

    // WitchPlayerState->OnDeath.AddUObject(this, &ThisClass::RestartIfNoLivingPlayers);
    WitchPlayerState->OnDeath.AddUObject(this, &ThisClass::EndDayIfNoLivingPlayers);

    AWitchForestGameState* WitchForestGameState = GetGameState<AWitchForestGameState>();
    if (!WitchForestGameState)
    {
        return;
    }

    // UE_LOGFMT(LogWitchForestGame, Warning, "WitchForestGameMode::PostLogin() PlayerState '{PlayerStateName}' connected", WitchPlayerState->GetName());
    if (WitchForestGameState->IsCurseActive())
    {
        FWitchForestMessage NewMessage;
        NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
        NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Afflicted;
        NewMessage.Source = this;
        WitchPlayerState->ClientBroadcastMessage(NewMessage);
    }
}

void AWitchForestGameMode::StartPlay()
{
    Super::StartPlay();

    // FTimerHandle RestartTimer;
    // GetWorld()->GetTimerManager().SetTimer(RestartTimer, FTimerDelegate::CreateUObject(this, &ThisClass::StartRound), RestartTime, false, -1.0f);
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

    /* if (!DefaultCurse)
    {
        UE_LOGFMT(LogWitchForestGame, Warning, "'{GameModeName}' Start round failed, no curse is set.", GetName());
        return;
    } */


    // WitchGameState->SetCurse(NewObject<UCurse>(this, DefaultCurse.Get(), TEXT("Default Curse")));
    WitchGameState->SetCurse(GenerateCurse());
    WitchGameState->CursePlayers();
    WitchGameState->CurseTimeRemaining = CurseTimeLimit;

    FWitchForestMessage NewMessage;
    NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
    NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Afflicted;
    NewMessage.Source = this;
    BroadcastMessageAllClients(NewMessage);

    WitchGameState->Phase = EWitchForestGamePhase::Nighttime;
    // StartDay();
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

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "'{GameModeName}' StartDay: The world was invalid.", GetName());
        return;
    }

    UE_LOGFMT(LogWitchForestGame, Display, "'{GameModeName}' Day started. {DayCount} days remain.", GetName(), WitchGameState->CurseTimeRemaining);

    World->GetTimerManager().SetTimer(WitchGameState->CurrentDayTimer, FTimerDelegate::CreateUObject(this, &ThisClass::EndDay), DayLengthSeconds, false, -1.0f);
    WitchGameState->CurrentDayEndTimeServer = WitchGameState->GetServerWorldTimeSeconds() + DayLengthSeconds;
    WitchGameState->Phase = EWitchForestGamePhase::Daytime;

    // See the note below about MulticastEndDay
    WitchGameState->MulticastStartDay();
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
        WitchGameState->Phase = EWitchForestGamePhase::Intermission;
        FTimerHandle IntermissionTimer;
        GetWorld()->GetTimerManager().SetTimer(IntermissionTimer, FTimerDelegate::CreateLambda([this, WitchGameState]()
            {
                if (WitchGameState)
                {
                    WitchGameState->Phase = EWitchForestGamePhase::Nighttime;
                    RespawnDeadPlayers();
                }
            }), IntermissionTime, false, -1.0f);
    }

    // Invoke the replicated EndDay function. This needs to be called on the game state, since the game mode can't replicate functions
    // as it doesn't exist on the clients. (Only a CDO exists)
    WitchGameState->MulticastEndDay();
}

void AWitchForestGameMode::RequestStartDay()
{
    AWitchForestGameState* WitchForestGameState = GetGameState<AWitchForestGameState>();
    if (!WitchForestGameState)
    {
        return;
    }

    if (WitchForestGameState->Phase == EWitchForestGamePhase::Nighttime)
    {
        StartDay();
    }
}


void AWitchForestGameMode::EndGame()
{
    KillAllPlayers();
    FWitchForestMessage NewMessage;
    NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
    NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Lethal;
    NewMessage.Source = this;
    BroadcastMessageAllClients(NewMessage);

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

void AWitchForestGameMode::BroadcastMessageAllClients(const FWitchForestMessage& Message)
{
    AWitchForestGameState* WitchForestGameState = GetGameState<AWitchForestGameState>();
    if (!WitchForestGameState)
    {
        UE_LOGFMT(LogWitchForestGame, Error, "WitchForestGameMode::BroadcastMessageAllClients failed. The game state was invalid.");
        return;
    }

    for (APlayerState* PlayerState : WitchForestGameState->PlayerArray)
    {
        AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(PlayerState);
        if (!WitchPlayerState)
        {
            continue;
        }

        WitchPlayerState->ClientBroadcastMessage(Message);
    }

    // Broadcast the message to the host, in case this is a listen server
    UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
    MessageSystem.BroadcastMessage(Message.Verb, Message);
}

UCurse* AWitchForestGameMode::GenerateCurse() const
{
    if (!CurrentItemSet)
    {
        UE_LOGFMT(LogWitchForestGame, Warning, "WitchForestGameMode GenerateCurse() failed. CurrentItemSet was invalid.");
        return nullptr;
    }

    if (CurrentItemSet->Items.Num() == 0)
    {
        UE_LOGFMT(LogWitchForestGame, Warning, "WitchForestGameMode GenerateCurse() failed. CurrentItemSet '{ItemSetname}' does not have any valid entries.", CurrentItemSet->GetName());
        return nullptr;
    }

    UCurse* NewCurse = NewObject<UCurse>();
    TArray<FGameplayTag> ChosenIngredients;

    const uint8 NumIngredients = FMath::RandRange(1, 5);
    for (int i = 0; i < NumIngredients; ++i)
    {
        const uint8 IngredientIndex = FMath::RandRange(0, CurrentItemSet->Items.Num() - 1);
        ChosenIngredients.Add(CurrentItemSet->Items[IngredientIndex].ItemTag);
    }

    NewCurse->SetRequiredItems(ChosenIngredients);
    return NewCurse;
}
