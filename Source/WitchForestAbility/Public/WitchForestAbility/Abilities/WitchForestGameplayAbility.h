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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AbilityActivation, meta = (AllowPrivateAccess = true))
	uint8 Priority = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AbilityActivation, meta = (AllowPrivateAccess = true))
	bool bConsumeInput = false;

public:
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	uint8 GetPriority() const { return Priority; }

	bool ConsumesInput() const { return bConsumeInput; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EAbilityActivationPolicy ActivationPolicy;

	// Get the owning pawn, as the owning actor might be a player state
	// @returns Owning pawn, or nullptr if none can be found
	static APawn* GetOwningPawnFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo);
};
