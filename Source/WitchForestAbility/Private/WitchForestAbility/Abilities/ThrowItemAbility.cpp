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
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "ThrowItemAbility failed. Could not find pawn.");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
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

		ThrownItem->SetLastHeldASC(GetAbilitySystemComponentFromActorInfo());
		ThrownItem->SetVelocity(CurrentPawnVelocity * 1.5f);

		/*
		if (IsLocallyControlled())
		{
			ThrownItem->ServerSetLocationAndVelocity(ItemHandle->GetComponentLocation(), CurrentPawnVelocity * 1.5f, ActorInfo->AvatarActor.Get());
			FTimerHandle TimerHandle;
			FTimerDelegate EnableReplicationDelegate;
			EnableReplicationDelegate.BindLambda([ThrownItem]()
				{
					// ThrownItem->ServerSetLocationAndVelocity(ThrownItem->GetActorLocation(), ThrownItem->GetVelocity(), nullptr);
					ThrownItem->SetOwner(nullptr);
				});
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, EnableReplicationDelegate, 1.0f, false);
		}*/
	}
	else
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "ThrowItemAbility failed. Unable to consume pickup.");
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

bool UThrowItemAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		return false;
	}

	UItemHandleComponent* ItemHandleComponent = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandleComponent)
	{
		return false;
	}
	return ItemHandleComponent->HoldingItem();
}
