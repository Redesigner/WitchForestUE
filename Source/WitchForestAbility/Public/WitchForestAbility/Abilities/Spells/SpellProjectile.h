// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h" 

#include "SpellProjectile.generated.h"

UCLASS()
class WITCHFORESTABILITY_API ASpellProjectile : public AActor
{
	GENERATED_BODY()

	FGameplayEffectSpecHandle EffectHandle;

	TArray<TWeakObjectPtr<AActor>> ActorsHit;

public:	
	ASpellProjectile();

	void Tick(float DeltaTime) override;

	void SetEffectHandle(FGameplayEffectSpecHandle InHandle);

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	void ApplyGameplayEffectToTarget(AActor* Target);
};
