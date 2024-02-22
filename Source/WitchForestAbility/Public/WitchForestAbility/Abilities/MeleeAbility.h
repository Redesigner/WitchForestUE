// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "MeleeAbility.generated.h"

class UMeleeComponent;
class UGameplayEffect;

UCLASS()
class WITCHFORESTABILITY_API UMeleeAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAnimMontage> MeleeAnimation;

	// A reference to the component, so it's easier to access from delegates
	TWeakObjectPtr<UMeleeComponent> MeleeComponent;
	FDelegateHandle ActorHitDelegateHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<UGameplayEffect>> EffectsToApply;

	TArray<TWeakObjectPtr<AActor>> DamagedActors;

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;
	
public:
	UFUNCTION()
	void ForceEndAbility();

	void OnEnemyHit(AActor* Enemy);
};
