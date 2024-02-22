// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WitchForestGameplayAbility.generated.h"


UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	OnInputPressed,
	Repeated,
	OnSpawn
};

UCLASS()
class WITCHFORESTABILITY_API UWitchForestGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EAbilityActivationPolicy ActivationPolicy;

	// Get the owning pawn, as the owning actor might be a player state
	// @returns Owning pawn, or nullptr if none can be found
	APawn* GetOwningPawnFromActorInfo() const;
};
