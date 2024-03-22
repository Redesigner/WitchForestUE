// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h" 

#include "GameplayEffectTypes.h" 

#include "SpellProjectile.generated.h"

UCLASS()
class WITCHFORESTABILITY_API ASpellProjectile : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayTags, meta = (AllowPrivateAccess = true))
	FGameplayTagContainer GameplayTags;

	FGameplayEffectSpecHandle EffectHandle;

	TArray<TWeakObjectPtr<AActor>> ActorsHit;

	// The actor that owns this projectile. Filtered out by collision events
	TWeakObjectPtr<AActor> OwningActor;


	void Tick(float DeltaTime) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	ASpellProjectile();

	void SetEffectHandle(FGameplayEffectSpecHandle InHandle);

	void SetOwningActor(AActor* Actor);

	void SetVelocity(FVector NewVelocity);

protected:
	void ApplyGameplayEffectToTarget(AActor* Target, FHitResult HitResult);
};
