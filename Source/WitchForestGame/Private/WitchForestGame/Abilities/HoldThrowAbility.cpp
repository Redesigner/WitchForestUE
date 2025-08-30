// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/HoldThrowAbility.h"

#include "WitchForestGame/WitchForestASC.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"

void UHoldThrowAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	GetWorld()->GetTimerManager().SetTimer(HoldTimer, MaxHoldTime, false);
}

bool UHoldThrowAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

void UHoldThrowAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(HoldTimer);

	FGameplayEventData Payload;
	Payload.EventTag = WitchForestGameplayTags::GameplayEvent_AbilityTrigger_Throw;
	Payload.Instigator = ActorInfo->OwnerActor.Get();
	Payload.Target = ActorInfo->OwnerActor.Get();
	Payload.EventMagnitude = 1.0f - FMath::Clamp(RemainingTime / MaxHoldTime, 0.0f, 1.0f);

	ActorInfo->AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
