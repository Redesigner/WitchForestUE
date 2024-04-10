// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h" 

#include "Pickup.generated.h"

class UWitchForestAbilitySet;
class UAbilitySystemComponent;
class USphereComponent;

UCLASS()
class WITCHFORESTGAME_API APickup : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestAbilitySet> GrantedAbilitySet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomReplication, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float MaxTeleportDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomReplication, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float InterpolationRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayTags, meta = (AllowPrivateAccess = true))
	FGameplayTagContainer GameplayTags;


	float CurrentError = 0.0f;

	TWeakObjectPtr<APickup> FakeOwner;

	bool bIsFake = false;

	bool bIsShadowingReal = false;


	void Tick(float DeltaSeconds) override;

	void OnRep_ReplicatedMovement() override;

	void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

protected:
	// The ASC of whichever actor held this last. This can be used to infer that they caused any sort of damage or other side effects
	TWeakObjectPtr<UAbilitySystemComponent> LastHolder;

	bool bThrown = false;

public:
	APickup();

	void EnableMovement();

	void DisableMovement();

	UFUNCTION(Server, Unreliable)
	void ServerSetLocationAndVelocity(FVector Location, FVector Velocity, AActor* InstigatorAvatar);

	UFUNCTION(Server, Reliable)
	void ServerRequestChangeOwner(AActor* NewOwner);

	UFUNCTION(Client, Reliable)
	void ClientChangeOwner(AActor* NewOwner);

	void AddImpulse(FVector Impulse);

	void SetVelocity(FVector Velocity);

	void SetThrown(bool bNewValue);

	void DisableReplication();

	UWitchForestAbilitySet* GetGrantedAbilitySet() const;

	// @TODO: Make a more robust system
	bool bHeld = false;

	bool IsFake() const;

	void SetLastHeldASC(UAbilitySystemComponent* ASC);

	void AttachFakeTo(APickup* Other);

	virtual void OnConsume();

	virtual bool CanBeStored() const;

	APickup* GetFakeOwner() const;
};