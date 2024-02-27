// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/PickupItemAbility.h"

#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "Components/SphereComponent.h"
#include "GameFramework/PlayerState.h"

void UPickupItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();

	APlayerState* PlayerState = Cast<APlayerState>(Owner);
	if (!PlayerState)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AWitch* Witch = Cast<AWitch>(PlayerState->GetPawn());
	if (!Witch)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UItemHandleComponent* ItemHandle = Witch->GetComponentByClass<UItemHandleComponent>();
	USphereComponent* InteractVolume = Witch->GetInteractionVolume();
	if (!ItemHandle || ItemHandle->HoldingItem() || !InteractVolume)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TSet<AActor*> OverlappingActors;
	InteractVolume->GetOverlappingActors(OverlappingActors, APickup::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		APickup* OverlappingPickup = Cast<APickup>(OverlappingActor);
		if (!OverlappingPickup || OverlappingPickup->bHeld)
		{
			continue;
		}
		ItemHandle->PickupItem(OverlappingPickup);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
