// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/ControlTipsViewModel.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "WitchForestAbility/Input/WitchForestInputHints.h"
#include "WitchForestGame/Character/WitchPlayerController.h"


void UControlTipsViewModel::PotentialAbilityActivationChanged()
{
	if (!OwningASC.IsValid())
	{
		return;
	}

	if (!InputHints)
	{
		return;
	}

	TArray<TObjectPtr<UControlTipUiSet>> ControlTipsTemp;

	for (FGameplayTag& InputTag : WatchedInputTags)
	{
		FGameplayAbilitySpec Spec;
		if (!OwningASC->GetFirstAbilityForInputTag(InputTag, Spec))
		{
			continue;
		}

		UWitchForestGameplayAbility* Ability = Cast<UWitchForestGameplayAbility>(Spec.Ability);
		if (!Ability)
		{
			continue;
		}

		UControlTipUiSet* ControlTip = NewObject<UControlTipUiSet>();
		ControlTip->ControlTipText = Ability->GetTipName();
		ControlTip->DisplayIcon = InputHints->FindGamepadButtonIconForInputTag(InputTag);

		ControlTipsTemp.Add(ControlTip);
	}

	ControlHints = ControlTipsTemp;
	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::ControlHints);
	// UE_MVVM_SET_PROPERTY_VALUE(ControlHints, ControlTipsTemp);
}

void UControlTipsViewModel::InventoryChanged(FGameplayTag Item, uint8 SlotIndex)
{
	PotentialAbilityActivationChanged();
}

void UControlTipsViewModel::BindAbilitySystem(UWitchForestASC* AbilitySystem, AWitchPlayerController* PlayerController, TArray<FGameplayTag> WatchedTags)
{
	PlayerController->OnPotentialAbilityActivationChanged.AddUObject(this, &ThisClass::PotentialAbilityActivationChanged);

	OwningASC = AbilitySystem;
	WatchedInputTags = WatchedTags;

	if (OwningASC.IsValid())
	{
		OwningASC->OnAbilitiesChanged.AddUObject(this, &ThisClass::PotentialAbilityActivationChanged);
	}

	PotentialAbilityActivationChanged();
}

void UControlTipsViewModel::SetInputHints(UWitchForestInputHints* InputHintsIn)
{
	InputHints = InputHintsIn;
}
