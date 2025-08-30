// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Attributes/WitchAttributeSet.h"

#include "Net/UnrealNetwork.h"


UWitchAttributeSet::UWitchAttributeSet()
{
	HarvestPower.SetBaseValue(100.0f);
	HarvestPower.SetCurrentValue(100.0f);

	SpellPower.SetBaseValue(100.0f);
	SpellPower.SetCurrentValue(100.0f);

	Mana.SetBaseValue(100.0f);
	Mana.SetCurrentValue(100.0f);

	MaxMana.SetBaseValue(100.0f);
	MaxMana.SetCurrentValue(100.0f);

	ManaRegenRate.SetBaseValue(1.0f);
	ManaRegenRate.SetCurrentValue(1.0f);
}

void UWitchAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UWitchAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWitchAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UWitchAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetManaAttribute() || Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxMana.GetCurrentValue());
		return;
	}
}

void UWitchAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWitchAttributeSet, Mana, OldMana);
}

void UWitchAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWitchAttributeSet, MaxMana, OldMaxMana);
}
