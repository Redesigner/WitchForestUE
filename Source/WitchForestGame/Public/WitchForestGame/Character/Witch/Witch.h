// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"

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
class WITCHFORESTGAME_API AWitch : public ACharacter
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

public:
	AWitch(const FObjectInitializer& ObjectInitializer);

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void BindActions(UInputComponent* PlayerInputComponent);


	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);


	// Native Actions
	void Move(const FInputActionInstance& Instance);

	void ShiftSlot(const FInputActionInstance& Instance);


	// Consider making Interaction Volume a seprate component so we can use GetComponentByClass instead
	USphereComponent* GetInteractionVolume() const;

private:
	UFUNCTION(BlueprintCallable)
	UWitchForestASC* GetWitchForestASC() const;
};