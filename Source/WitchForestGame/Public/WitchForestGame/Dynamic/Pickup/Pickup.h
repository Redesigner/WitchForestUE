// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Pickup.generated.h"

class UWitchForestAbilitySet;
class UAbilitySystemComponent;
class USphereComponent;

UCLASS()
class WITCHFORESTGAME_API APickup : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestAbilitySet> GrantedAbilitySet;

protected:
	// The ASC of whichever actor held this last. This can be used to infer that they caused any sort of damage or other side effects
	TWeakObjectPtr<UAbilitySystemComponent> LastHolder;

public:
	APickup();

	void EnableMovement();

	void DisableMovement();

	void AddImpulse(FVector Impulse);

	void SetVelocity(FVector Velocity);

	UWitchForestAbilitySet* GetGrantedAbilitySet() const;

	// @TODO: Make a more robust system
	bool bHeld = false;

	void SetLastHeldASC(UAbilitySystemComponent* ASC);
};