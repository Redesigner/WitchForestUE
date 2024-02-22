// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"

#include "GameFramework/PlayerState.h"

APawn* UWitchForestGameplayAbility::GetOwningPawnFromActorInfo() const
{
    AActor* Actor = GetOwningActorFromActorInfo();
    if (APawn* Pawn = Cast<APawn>(Actor))
    {
        return Pawn;
    }
    if (APlayerState* PlayerState = Cast<APlayerState>(Actor))
    {
        return PlayerState->GetPawn();
    }
    return nullptr;
}
