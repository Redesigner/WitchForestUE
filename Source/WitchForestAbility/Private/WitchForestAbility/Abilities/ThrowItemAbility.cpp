// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/ThrowItemAbility.h"

#include "GameFramework/PlayerState.h"

#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Pickup.h"

void UThrowItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Pawn = Owner;

	// Since the ASC can live on the PlayerState for player characters, check here and get their pawns.
	// Otherwise, the ASC should be owned by the pawn -- for enemies and other NPCs
	if (APlayerState* PlayerState = Cast<APlayerState>(Owner))
	{
		Pawn = PlayerState->GetPawn();
	}
	UItemHandleComponent* ItemHandle = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (APickup* ThrownItem = ItemHandle->ConsumeItem())
	{
		ThrownItem->AddImpulse(Pawn->GetActorForwardVector() * 10000.0f);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
