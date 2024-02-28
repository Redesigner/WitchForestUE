// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/InteractAbility.h"

#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestAbility/WitchForestASC.h"

void UInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	APawn* OwningPawn = GetOwningPawnFromActorInfo(ActorInfo);
	if (!OwningPawn)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UItemHandleComponent* ItemHandleComponent = OwningPawn->GetComponentByClass<UItemHandleComponent>();
	if (ItemHandleComponent && ItemHandleComponent->HoldingItem())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		UWitchForestASC* ASC = Cast<UWitchForestASC>(GetAbilitySystemComponentFromActorInfo());
		ASC->TryActivateAbilitiesByTag(HeldItemAbilityTag);
		return;
	}

	/*if (AWitch* Witch = Cast<AWitch>(OwningPawn))
	{
		Witch->GetInteractionVolume()
	}*/
}
