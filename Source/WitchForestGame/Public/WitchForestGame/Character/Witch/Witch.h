// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"

#include "Witch.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UWitchForestInputConfig;
class UWitchForestASC;
class UInventoryComponent;
class UItemHandleComponent;
class USphereComponent;
struct FInputActionInstance;

UCLASS()
class WITCHFORESTGAME_API AWitch : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UItemHandleComponent> ItemHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> InteractionVolume;


	DECLARE_MULTICAST_DELEGATE(FOnPotentialInteractionsChanged);


	AWitch(const FObjectInitializer& ObjectInitializer);

	void BindActions(UInputComponent* PlayerInputComponent);

	void Tick(float DeltaSeconds) override;
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void PossessedBy(AController* NewController) override;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	// Native Actions
	void Move(const FInputActionInstance& Instance);
	void Look(const FInputActionInstance& Instance);
	void ShiftSlot(const FInputActionInstance& Instance);

	void HeldItemChanged();


	UFUNCTION(BlueprintCallable)
	UWitchForestASC* GetWitchForestASC() const;

	void TeleportToStart();

	UFUNCTION()
	void InteractVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InteractVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	// Consider making Interaction Volume a seprate component so we can use GetComponentByClass instead
	USphereComponent* GetInteractionVolume() const;

	/// Set the player's color. Must be implemented in blueprint, since mesh refs are difficult
	UFUNCTION(BlueprintImplementableEvent)
	void SetPlayerColor(FColor Color);

	FOnPotentialInteractionsChanged OnPotentialInteractionsChanged;
};