// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/WithdrawItemAbility.h"

#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"

void UWithdrawItemAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Pawn = Owner;

	UInventoryComponent* Inventory = Owner->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (APlayerState* PlayerState = Cast<APlayerState>(Owner))
	{
		Pawn = PlayerState->GetPawn();
	}
	// @TODO: This should be packed into the ability activation request by the client, since the server won't know which item is selected
	uint8 SelectedIndex = 0;
	FGameplayTag HeldItemTag = Inventory->ConsumeItem(SelectedIndex);
	if (HeldItemTag.MatchesTagExact(FGameplayTag::EmptyTag))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectSpecHandle HeldEffectSpec = MakeOutgoingGameplayEffectSpec(ItemHeldEffect);
	HeldEffectSpec.Data->AddDynamicAssetTag(HeldItemTag);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*HeldEffectSpec.Data);
	UE_LOG(LogTemp, Display, TEXT("Withdrawing item %s"), *HeldItemTag.GetTagName().ToString());
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
