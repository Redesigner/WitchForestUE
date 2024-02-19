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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	float Health = AttributeSet->GetHealthAttribute().GetGameplayAttributeData(AttributeSet)->GetCurrentValue();
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, Health);
}

void UHealthViewModel::HealthChanged(const FOnAttributeChangeData& Data)
{
	float NewCurrentHealth = Data.NewValue;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, NewCurrentHealth);
}
