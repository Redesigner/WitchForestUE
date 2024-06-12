// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"

#include "WitchForestAbility/Effects/GameplayEffectContainer.h"

#include "SpellAbility.generated.h"

class UGameplayEffect;
class UGameplayEffectContainer;
class ASpellProjectile;

UCLASS()
class WITCHFORESTABILITY_API USpellAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = true))
	FGameplayEffectContainer EffectContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = true))
	TSubclassOf<ASpellProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float ProjectileSpeed = 800.0f;


	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;


	void SpawnProjectile(FVector Location);
};
