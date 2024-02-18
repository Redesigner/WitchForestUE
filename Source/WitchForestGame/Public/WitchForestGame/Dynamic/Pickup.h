// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Pickup.generated.h"

class UWitchForestAbilitySet;
class UWitchForestASC;
class USphereComponent;

UCLASS()
class WITCHFORESTGAME_API APickup : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestAbilitySet> GrantedAbilitySet;

public:
	APickup();

	void EnableMovement();

	void DisableMovement();

	void AddImpulse(FVector Impulse);

	UWitchForestAbilitySet* GetGrantedAbilitySet() const;

	// @TODO: Make a more robust system
	bool bHeld = false;
};