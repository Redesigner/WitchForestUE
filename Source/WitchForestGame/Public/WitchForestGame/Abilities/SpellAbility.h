// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestGame/Abilities/WitchForestGameplayAbility.h"

#include "WitchForestGame/Effects/GameplayEffectContainer.h"

#include "SpellAbility.generated.h"

class UGameplayEffect;
class UGameplayEffectContainer;
class ASpellProjectile;

UCLASS()
class WITCHFORESTGAME_API USpellAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = true))
	FGameplayEffectContainer EffectContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = true))
	TSubclassOf<ASpellProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float ProjectileSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Activation, meta = (AllowPrivateAccess = true))
	bool bEndAbilityOnFire = true;


	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;


	void SpawnProjectile(FVector Location);
};
