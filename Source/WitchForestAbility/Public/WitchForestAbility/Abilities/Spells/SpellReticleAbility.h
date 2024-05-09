// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "SpellReticleAbility.generated.h"


class AGameplayAbilityTargetActor;
class ASpellProjectile;

UCLASS()
class WITCHFORESTABILITY_API USpellReticleAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Targeting, meta = (AllowPrivateAccess = true))
	TSubclassOf<AGameplayAbilityTargetActor> TargetingClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UGameplayEffect>> SpellEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess = true))
	float DamageAmount = 0.0f;

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
