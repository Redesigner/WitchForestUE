// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/ManaViewModel.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/WitchAttributeSet.h"

void UManaViewModel::BindAttributeSet(UWitchForestASC* AbilitySystemComponent, UWitchAttributeSet* AttributeSet)
{
	if (!AbilitySystemComponent || !AttributeSet)
	{
		return;
	}
	// Bind our functions to the attributeset's appropriate delegates
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);

	// Broadcast changes for our initial value setting
	UE_MVVM_SET_PROPERTY_VALUE(CurrentMana, AttributeSet->GetManaAttribute().GetGameplayAttributeData(AttributeSet)->GetCurrentValue());
	UE_MVVM_SET_PROPERTY_VALUE(MaxMana, AttributeSet->GetMaxManaAttribute().GetGameplayAttributeData(AttributeSet)->GetCurrentValue());
}

void UManaViewModel::ManaChanged(const FOnAttributeChangeData& Data)
{
	float NewCurrentMana = Data.NewValue;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentMana, NewCurrentMana);
}

void UManaViewModel::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	float NewMaxMana = Data.NewValue;
	UE_MVVM_SET_PROPERTY_VALUE(MaxMana, NewMaxMana);
}
