// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameplayEffectTypes.h" 
#include "ActiveGameplayEffectHandle.h"

#include "EffectApplicationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTABILITY_API UEffectApplicationComponent : public UActorComponent
{
	GENERATED_BODY()

	TArray<FGameplayEffectSpecHandle> EffectHandles;

	// The actor that set our effects. Filtered out by collision events
	TWeakObjectPtr<AActor> OwningActor;

public:
	void SetEffectHandles(AActor* OwningActorIn, TArray<FGameplayEffectSpecHandle> InHandles);

	TArray<FActiveGameplayEffectHandle> ApplyGameplayEffectsToTarget(AActor* Target, FHitResult HitResult);

	AActor* GetOwningActor() const { return OwningActor.Get(); }
};
