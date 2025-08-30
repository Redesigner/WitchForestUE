// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h" 

#include "SpellProjectile.generated.h"

class UEffectApplicationComponent;

UCLASS()
class WITCHFORESTGAME_API ASpellProjectile : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEffectApplicationComponent> EffectApplication;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	FVector Velocity;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bAffectedByGravity = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bDestroyOnBlockingHit = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true))
	bool bDestroyOnEffectApplied = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayTags, meta = (AllowPrivateAccess = true))
	FGameplayTagContainer GameplayTags;

public:	
	ASpellProjectile();

	void SetOwningActor(AActor* Actor);

	void SetVelocity(FVector NewVelocity);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void HitWall(FHitResult Hit);

private:
	TArray<TWeakObjectPtr<AActor>> ActorsHit;


	void Tick(float DeltaTime) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
