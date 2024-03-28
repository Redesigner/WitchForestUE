// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_SpellTrace.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTABILITY_API ATargetActor_SpellTrace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Targeting, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float InitialTargetingDistance = 400.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Targeting, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float TraceDistance = 1000.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Targeting, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	FCollisionProfileName TraceProfile;
	

	FVector WorldOffset;

	TWeakObjectPtr<AActor> SourceActor;


	ATargetActor_SpellTrace();

	void Tick(float DeltaSeconds) override;

	void StartTargeting(UGameplayAbility* Ability) override;

	void ConfirmTargetingAndContinue() override;

	FGameplayAbilityTargetDataHandle MakeTargetData(const FHitResult& HitResult) const;

	FHitResult PerformTrace();
};
