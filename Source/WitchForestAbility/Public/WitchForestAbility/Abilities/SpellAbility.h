// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"

#include "SpellAbility.generated.h"

class UGameplayEffect;
class ASpellProjectile;

UCLASS()
class WITCHFORESTABILITY_API USpellAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> SpellEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess = true))
	float DamageAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = true))
	TSubclassOf<ASpellProjectile> ProjectileClass;


	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;

	void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

};
