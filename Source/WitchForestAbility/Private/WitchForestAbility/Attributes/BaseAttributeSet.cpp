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

void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth);
}