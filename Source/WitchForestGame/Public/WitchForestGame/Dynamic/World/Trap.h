// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

class UGameplayEffect;

UCLASS()
class WITCHFORESTGAME_API ATrap : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> TrapEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess = true))
	float Damage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cooldown, meta = (AllowPrivateAccess = true))
	bool bUsesCooldown = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cooldown, meta = (EditCondition = bUsesCooldown, AllowPrivateAccess = true))
	float CooldownTime = 5.0f;


	bool bOnCooldown = false;

	FTimerHandle CooldownTimer;

	
	ATrap();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void StartCooldown();

	void EndCooldown();

	void ApplyEffectToOverlappingActors();

	bool ApplyEffectToActor(AActor* Target);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnStartCooldown();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndCooldown();
};
