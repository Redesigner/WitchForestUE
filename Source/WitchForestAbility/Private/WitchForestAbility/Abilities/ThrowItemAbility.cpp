// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/ThrowItemAbility.h"

#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"
#include "GameFramework/MovementComponent.h" 

#include "WitchForestAbility.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

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
		UE_LOGFMT(LogWitchForestAbility, Warning, "ThrowItemAbility failed. '{PawnName}' does not have an ItemHandleComponent.", Pawn->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (APickup* ThrownItem = ItemHandle->ConsumeItem())
	{
		FVector CurrentPawnVelocity;
		if (UMovementComponent* MovementComponent = Pawn->GetComponentByClass<UMovementComponent>())
		{
			CurrentPawnVelocity = MovementComponent->Velocity;
		}
		ThrownItem->SetVelocity(CurrentPawnVelocity);
		ThrownItem->AddImpulse(Pawn->GetActorForwardVector() * 10000.0f);
		ThrownItem->SetLastHeldASC(GetAbilitySystemComponentFromActorInfo());
	}
	else
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "ThrowItemAbility failed. Unable to consume pickup.");
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
