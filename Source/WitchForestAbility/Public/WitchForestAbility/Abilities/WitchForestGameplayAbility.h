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

	virtual void ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EAbilityActivationPolicy ActivationPolicy;

};
