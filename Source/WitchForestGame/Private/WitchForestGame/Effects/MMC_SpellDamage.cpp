// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Effects/MMC_SpellDamage.h"

#include "WitchForestGame/Attributes/WitchAttributeSet.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

UMMC_SpellDamage::UMMC_SpellDamage()
{
	SpellPowerDef.AttributeToCapture = UWitchAttributeSet::GetSpellPowerAttribute();
	SpellPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	SpellPowerDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(SpellPowerDef);
}

float UMMC_SpellDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluationParameters;
	float SpellPower = 0.0f;
	GetCapturedAttributeMagnitude(SpellPowerDef, Spec, EvaluationParameters, SpellPower);
	float SpellDamage = Spec.GetSetByCallerMagnitude(WitchForestGameplayTags::SetByCaller_Damage, true);

	return SpellDamage * SpellPower / 100.0f;
}
