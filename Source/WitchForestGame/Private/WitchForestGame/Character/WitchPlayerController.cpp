// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerController.h"

#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Dynamic/Brewing/PotionRecipeSet.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Game/WitchForestCheatManager.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Camera/CameraComponent.h"

AWitchPlayerController::AWitchPlayerController()
{
    CheatClass = UWitchForestCheatManager::StaticClass();
}


void AWitchPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (AWitch* Witch = Cast<AWitch>(InPawn))
    {
        Witch->OnPotentialInteractionsChanged.AddUObject(this, &ThisClass::PotentialInteractionsChanged);
    }
}

void AWitchPlayerController::BeginPlayingState()
{
    Super::BeginPlayingState();

    if (!IsLocalPlayerController())
    {
        return;
    }

    if (AWitchPlayerState* WitchPlayerState = GetPlayerState<AWitchPlayerState>())
    {
        WitchPlayerState->GrantAbilities();
        WitchPlayerState->InitializeAttributes();

        if (IsLocalPlayerController())
        {
            WitchPlayerState->GetWitchForestASC()->RegisterGameplayTagEvent(WitchForestGameplayTags::GameplayEffect_Blind, EGameplayTagEventType::AnyCountChange).AddUObject(this, &ThisClass::BlindStacksChanged);
        }

        SetupUI(WitchPlayerState);

        if (RootWidgetClass)
        {
            UUserWidget* RootWidget = CreateWidget<UUserWidget>(this, RootWidgetClass.Get());
            RootWidget->AddToViewport();
        }
    }
}

void AWitchPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bIsBlind || !EffectParameterCollection.IsValid() || !GetPawn())
    {
        return;
    }

    if (!IsLocalPlayerController())
    {
        return;
    }

    UMaterialParameterCollectionInstance* CollectionInstance = GetWorld()->GetParameterCollectionInstance(EffectParameterCollection.Get());
    if (!CollectionInstance)
    {
        return;
    }

    CollectionInstance->SetVectorParameterValue("Center", GetPawn()->GetActorLocation());
}

void AWitchPlayerController::BeginPlay()
{
	Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (!InputMapping.IsNull())
            {
                InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 1);
            }
        }
    }

    UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
    MessageSubsystem.RegisterListener(WitchForestGameplayTags::Event_Discovery, this, &ThisClass::OnDiscoveryMessage);
}

UWitchForestASC* AWitchPlayerController::GetWitchForestASC() const
{
    if (AWitchPlayerState* WitchPlayerState = GetPlayerState<AWitchPlayerState>())
    {
        return WitchPlayerState->GetWitchForestASC();
    }

    return nullptr;
}


void AWitchPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PostProcessInput(DeltaTime, bGamePaused);

    UWitchForestASC* WitchForestASC = GetWitchForestASC();
    if (!WitchForestASC)
    {
        return;
    }

    WitchForestASC->ProcessAbilityInput(DeltaTime, bGamePaused);
}

void AWitchPlayerController::OverrideTeam(EWitchForestTeam NewTeam)
{
    if (IWitchForestTeamAgentInterface* WitchTeamInterface = GetPlayerState<AWitchPlayerState>())
    {
        WitchTeamInterface->OverrideTeam(NewTeam);
    }
}

void AWitchPlayerController::SetWitchForestTeam(EWitchForestTeam InTeam)
{
    if (IWitchForestTeamAgentInterface* WitchTeamInterface = GetPlayerState<AWitchPlayerState>())
    {
        WitchTeamInterface->SetWitchForestTeam(InTeam);
    }
}

EWitchForestTeam AWitchPlayerController::GetWitchForestTeam() const
{
    if (IWitchForestTeamAgentInterface* WitchTeamInterface = GetPlayerState<AWitchPlayerState>())
    {
        return WitchTeamInterface->GetWitchForestTeam();
    }

    return EWitchForestTeam::Unaffiliated;
}

void AWitchPlayerController::PotentialInteractionsChanged()
{
    OnPotentialAbilityActivationChanged.Broadcast();
}

void AWitchPlayerController::OnDiscoveryMessage(FGameplayTag Channel, const FWitchForestMessage& Payload)
{
    if (Payload.Verb == WitchForestGameplayTags::Event_Discovery_Recipe)
    {
        HandleRecipeDiscoveredMessage(Payload);
        return;
    }
}

void AWitchPlayerController::HandleRecipeDiscoveredMessage(const FWitchForestMessage& Payload)
{
    if (Payload.Source != GetPawn())
    {
        return;
    }

    AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
    if (!GameMode)
    {
        return;
    }

    UPotionRecipeSet* RecipeBook = GameMode->GetRecipeBook();
    UItemSet* ItemSet = GameMode->GetItemSet();
    if (!RecipeBook || !ItemSet)
    {
        return;
    }

    FPotionRecipe Recipe;
    if (RecipeBook->FindRecipeFromTag(Payload.Data, Recipe))
    {
        FInventoryItemData ItemData;
        ItemSet->FindItemDataForTag(Recipe.ResultItem, ItemData);

        DisplayRecipeDiscoveredMessage(Recipe, ItemData);
    }
}

void AWitchPlayerController::BlindStacksChanged(const FGameplayTag Tag, int32 Count)
{
    if (!BlindEffectClass)
    {
        return;
    }

    if (!GetPawn())
    {
        return;
    }

    UCameraComponent* Camera = GetPawn()->GetComponentByClass<UCameraComponent>();
    if (!Camera)
    {
        return;
    }

    if (Count > 0 && !bIsBlind)
    {
        bIsBlind = true;
        Camera->PostProcessSettings.AddBlendable(BlindEffectClass, 1.0f);
    }

    if (Count <= 0 && bIsBlind)
    {
        bIsBlind = false;
        Camera->PostProcessSettings.RemoveBlendable(BlindEffectClass);
    }
}
