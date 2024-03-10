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

	if (ItemHandle->HoldingItem())
	{
		ThrowItem(ItemHandle->GetHeldItem(), Pawn, ItemHandle);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "ThrowItemAbility failed. Unable to consume pickup.");
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
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


void UThrowItemAbility::ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey)
{
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		return;
	}

	UItemHandleComponent* ItemHandleComponent = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandleComponent)
	{
		return;
	}

	ItemHandleComponent->SetHeldItemHidden(false);

	if (!ItemHandleComponent->GetFakePickup())
	{
		return;
	}
	ItemHandleComponent->GetFakePickup()->Destroy();
}


void UThrowItemAbility::ActivateAbilitySucceed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FPredictionKey& PredictionKey)
{
	APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Pawn)
	{
		return;
	}

	UItemHandleComponent* ItemHandleComponent = Pawn->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandleComponent)
	{
		return;
	}

	if (APickup* HeldItem = ItemHandleComponent->ConsumeItem())
	{
		// HeldItem->SetActorHiddenInGame(false);
		HeldItem->SetOwner(nullptr);

		if (ItemHandleComponent->GetFakePickup())
		{
			// ItemHandleComponent->GetFakePickup()->Destroy();
			ItemHandleComponent->GetFakePickup()->AttachFakeTo(HeldItem);
		}
	}
}


void UThrowItemAbility::ThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle)
{
	if (IsPredictingClient())
	{
		SimulateThrowItem(Item, Pawn, ItemHandle);
	}
	else
	{
		ServerThrowItem(Item, Pawn, ItemHandle);
	}
}


void UThrowItemAbility::SimulateThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle)
{
	FVector CurrentPawnVelocity;
	if (UMovementComponent* MovementComponent = Pawn->GetComponentByClass<UMovementComponent>())
	{
		CurrentPawnVelocity = MovementComponent->Velocity;
	}

	APickup* FakePickup = GetWorld()->SpawnActorDeferred<APickup>(Item->GetClass(), Item->GetTransform());
	if (!FakePickup)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "ThrowItemAbility '{AbilityName}' failed to spawn fake pickup actor from template pickup '{PickupName}'.", GetName(), Item->GetName());
		return;
	}

	FakePickup->SetReplicates(false);
	FakePickup->SetActorLocation(ItemHandle->GetComponentLocation());
	FakePickup->FinishSpawning(Item->GetActorTransform());
	FakePickup->SetVelocity(CurrentPawnVelocity * 1.5f);
	ItemHandle->SetFakePickup(FakePickup);
	ItemHandle->SetHeldItemHidden(true);
}


void UThrowItemAbility::ServerThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle)
{
	FVector CurrentPawnVelocity;
	if (UMovementComponent* MovementComponent = Pawn->GetComponentByClass<UMovementComponent>())
	{
		CurrentPawnVelocity = MovementComponent->Velocity;
	}

	if (APickup* HeldItem = ItemHandle->ConsumeItem())
	{
		HeldItem->SetLastHeldASC(GetAbilitySystemComponentFromActorInfo());
		HeldItem->SetVelocity(CurrentPawnVelocity * 1.5f);
	}
}
