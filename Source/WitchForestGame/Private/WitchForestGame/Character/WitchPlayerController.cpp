// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerController.h"

#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestAbility/WitchForestASC.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AWitchPlayerController::AWitchPlayerController()
{
}


void AWitchPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (AWitchPlayerState* WitchPlayerState = GetPlayerState<AWitchPlayerState>())
    {
        WitchPlayerState->InitializeAttributes();
    }
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
