// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/WitchPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

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