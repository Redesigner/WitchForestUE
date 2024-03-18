// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/DrinkPotionAbility.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Pickup/Potion.h"

void UDrinkPotionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// These checks should be passed because of CanActivateAbility, but we want to be extra sure that there are no nullptrs,
	// so I think it's worth the extra complexity to be safe
	if (!ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	UItemHandleComponent* ItemHandle = Avatar->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APotion* Potion = Cast<APotion>(ItemHandle->GetHeldItem());
	if (!Potion)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ItemHandle->ConsumeItem()->Destroy();
	if (TSubclassOf<UGameplayEffect> PotionEffect = Potion->GetPotionEffect())
	{
		FGameplayEffectSpecHandle PotionEffectSpec = MakeOutgoingGameplayEffectSpec(PotionEffect);
		PotionEffectSpec.Data.Get()->SetSetByCallerMagnitude(WitchForestGameplayTags::SetByCaller_Damage, Potion->GetHealthAmount());
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, PotionEffectSpec);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

bool UDrinkPotionAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	AActor* Avatar = ActorInfo->AvatarActor.Get();
	UItemHandleComponent* ItemHandle = Avatar->GetComponentByClass<UItemHandleComponent>();
	if (!ItemHandle)
	{
		return false;
	}

	if (APotion* Potion = Cast<APotion>(ItemHandle->GetHeldItem()))
	{
		return true;
	}

	return false;
}
