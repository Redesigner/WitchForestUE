// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_SpellDamage.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UMMC_SpellDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
	FGameplayEffectAttributeCaptureDefinition SpellPowerDef;


	UMMC_SpellDamage();

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
