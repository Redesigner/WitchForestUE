// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WitchForestGameplayAbility.generated.h"


UENUM(BlueprintType)
enum class EAbilityActivationType : uint8
{
	OnInputPressed,
	Repeated
};

UCLASS()
class WITCHFORESTABILITY_API UWitchForestGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
};
