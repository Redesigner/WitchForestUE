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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	FVector Velocity;


	FGameplayEffectSpecHandle EffectHandle;

	TArray<TWeakObjectPtr<AActor>> ActorsHit;

	// The actor that owns this projectile. Filtered out by collision events
	TWeakObjectPtr<AActor> OwningActor;


	void Tick(float DeltaTime) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:	
	ASpellProjectile();

	void SetEffectHandle(FGameplayEffectSpecHandle InHandle);

	void SetOwningActor(AActor* Actor);

protected:
	void ApplyGameplayEffectToTarget(AActor* Target, FHitResult HitResult);
};
