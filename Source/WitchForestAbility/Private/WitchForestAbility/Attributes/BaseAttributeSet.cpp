// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 

UBaseAttributeSet::UBaseAttributeSet()
{
	MovementSpeed.SetBaseValue(600.0f);
	MovementSpeed.SetCurrentValue(600.0f);

	Health.SetBaseValue(100.0f);
	Health.SetCurrentValue(100.0f);

	MaxHealth.SetBaseValue(100.0f);
	MaxHealth.SetCurrentValue(100.0f);
}


void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Health, COND_None, REPNOTIFY_Always);
}


void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute.AttributeName == TEXT("Health"))
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			OnDeath.ExecuteIfBound(Data.EffectSpec);
		}
	}
}


void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth.GetCurrentValue());
		return;
	}
	if (Attribute == GetMaxHealthAttribute())
	{
		// If we decreased the max health, clamp our health so it isn't greater than max
		float CurrentHealth = Health.GetCurrentValue();
		if (NewValue < CurrentHealth)
		{
			Health.SetCurrentValue(NewValue);
		}
	}
}


void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth);
}


void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldMaxHealth);
}
