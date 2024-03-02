// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "Potion.generated.h"

class UGameplayEffect;

UCLASS()
class WITCHFORESTGAME_API APotion : public APickup
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect, meta = (AllowPrivateAccess = true))
	float SplashRadius = 150.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effect, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> SplashEffect;

	FTimerHandle BurstDebounceTimer;

    void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void Burst();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void HandleBurst();
};
