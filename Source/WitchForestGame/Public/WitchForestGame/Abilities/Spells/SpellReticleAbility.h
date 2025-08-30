// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestGame/Abilities/WitchForestGameplayAbility.h"

#include "WitchForestGame/Effects/GameplayEffectContainer.h"

#include "SpellReticleAbility.generated.h"


class AGameplayAbilityTargetActor;
class ASpellProjectile;

UCLASS()
class WITCHFORESTGAME_API USpellReticleAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Targeting, meta = (AllowPrivateAccess = true))
	TSubclassOf<AGameplayAbilityTargetActor> TargetingClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = true))
	FGameplayEffectContainer EffectContainer;

	UPROPERTY(EditDefaultsOnly, Category = Projectile, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float ProjectileSpawnHeight = 500.0f;


	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
	

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;
	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	UFUNCTION()
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData);

	void SpawnProjectile(FVector Location);
};
