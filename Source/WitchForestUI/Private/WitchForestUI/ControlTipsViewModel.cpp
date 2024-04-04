// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ControlTipsViewModel.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "WitchForestGame/Character/WitchPlayerController.h"


void UControlTipsViewModel::PotentialAbilityActivationChanged()
{
	if (!OwningASC.IsValid())
	{
		return;
	}

	FString ControllerHints;

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

		const FString TipFormatString = TEXT("Press '{0}' to '{1}'\n");
		FString ControllerTip = FString::Format(*TipFormatString, { InputTag.GetTagName().ToString(), Ability->GetTipName().ToString() });
		ControllerHints.Append(ControllerTip);
	}

	UE_MVVM_SET_PROPERTY_VALUE(ControlHints, FText::FromString(ControllerHints));
}

void UControlTipsViewModel::BindAbilitySystem(UWitchForestASC* AbilitySystem, AWitchPlayerController* PlayerController, TArray<FGameplayTag> WatchedTags)
{
	PlayerController->OnPotentialAbilityActivationChanged.AddUObject(this, &ThisClass::PotentialAbilityActivationChanged);

	OwningASC = AbilitySystem;
	WatchedInputTags = WatchedTags;
}
