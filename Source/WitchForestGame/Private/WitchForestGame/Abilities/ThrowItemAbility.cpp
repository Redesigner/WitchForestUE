// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/ThrowItemAbility.h"

#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"
#include "GameFramework/MovementComponent.h" 

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

UThrowItemAbility::UThrowItemAbility()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = WitchForestGameplayTags::GameplayEvent_AbilityTrigger_Throw;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

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

	if (!TriggerEventData)
	{
		UE_LOGFMT(LogWitchForestAbility, Warning, "ThrowItemAbility failed for '{PawnName}' ThrowItemAbility must be triggered by an event.", GetNameSafe(Pawn));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ItemHandle->HoldingItem())
	{
		ThrowItem(ItemHandle->GetHeldItem(), Pawn, ItemHandle, TriggerEventData->EventMagnitude);
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
		HeldItem->SetLastHeldASC(ActorInfo->AbilitySystemComponent.Get());

		if (ItemHandleComponent->GetFakePickup())
		{
			// ItemHandleComponent->GetFakePickup()->SetLastHeldASC(nullptr);
			ItemHandleComponent->GetFakePickup()->AttachFakeTo(HeldItem);
		}
		else
		{
			UE_LOGFMT(LogWitchForestAbility, Display, "ThrowAbility '{AbilityName}' succeeded in replication, but there was no real thrown item to attach the fake to.", GetName());
		}
	}
}


void UThrowItemAbility::ThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle, float Strength)
{
	if (IsPredictingClient())
	{
		SimulateThrowItem(Item, Pawn, ItemHandle, Strength);
	}
	else
	{
		ServerThrowItem(Item, Pawn, ItemHandle, Strength);
	}
}


void UThrowItemAbility::SimulateThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle, float Strength)
{
	APickup* FakePickup = GetWorld()->SpawnActorDeferred<APickup>(Item->GetClass(), Item->GetTransform());
	if (!FakePickup)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "ThrowItemAbility '{AbilityName}' failed to spawn fake pickup actor from template pickup '{PickupName}'.", GetName(), Item->GetName());
		return;
	}

	const float ThrowSpeed = MaxThrowSpeed * Strength;
	FakePickup->DisableReplication();
	FakePickup->bHeld = false;
	FakePickup->SetActorLocation(ItemHandle->GetComponentLocation());
	FakePickup->FinishSpawning(Item->GetActorTransform());
	FakePickup->SetVelocity(Pawn->GetActorForwardVector() * ThrowSpeed);
	FakePickup->SetLastHeldASC(GetAbilitySystemComponentFromActorInfo());

	if (ThrowSpeed >= MaxThrowSpeed / 2.0f)
	{
		FakePickup->SetThrown(true);
	}

	ItemHandle->SetFakePickup(FakePickup);
	ItemHandle->SetHeldItemHidden(true);
}


void UThrowItemAbility::ServerThrowItem(APickup* Item, APawn* Pawn, UItemHandleComponent* ItemHandle, float Strength)
{
	if (APickup* HeldItem = ItemHandle->ConsumeItem())
	{
		const float ThrowSpeed = MaxThrowSpeed * Strength;

		HeldItem->SetLastHeldASC(GetAbilitySystemComponentFromActorInfo());
		HeldItem->SetVelocity(Pawn->GetActorForwardVector() * ThrowSpeed);


		if (ThrowSpeed >= MaxThrowSpeed / 2.0f)
		{
			HeldItem->SetThrown(true);
		}
	}
}
