// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/HealthViewModel.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"


void UHealthViewModel::BindAttributeSet(UWitchForestASC* AbilitySystemComponent, UBaseAttributeSet* AttributeSet)
{
	if (!AbilitySystemComponent || !AttributeSet)
	{
		return;
	}
	// Bind our functions to the attributeset's appropriate delegates
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);

	// Broadcast changes for our initial value setting
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, AttributeSet->GetHealthAttribute().GetGameplayAttributeData(AttributeSet)->GetCurrentValue());
	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, AttributeSet->GetMaxHealthAttribute().GetGameplayAttributeData(AttributeSet)->GetCurrentValue());
}

void UHealthViewModel::HealthChanged(const FOnAttributeChangeData& Data)
{
	float NewCurrentHealth = Data.NewValue;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, NewCurrentHealth);
}

void UHealthViewModel::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, Data.NewValue);
}
