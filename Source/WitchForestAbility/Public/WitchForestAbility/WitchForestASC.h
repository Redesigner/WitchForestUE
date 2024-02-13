// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "WitchForestASC.generated.h"

UCLASS()
class WITCHFORESTABILITY_API UWitchForestASC : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UWitchForestASC();

	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
};