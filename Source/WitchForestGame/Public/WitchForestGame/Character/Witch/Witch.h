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

public:
	AWitch(const FObjectInitializer& ObjectInitializer);

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void BindActions(UInputComponent* PlayerInputComponent);


	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);


	void Move(const FInputActionInstance& Instance);

private:
	UWitchForestASC* GetWitchForestASC() const;
};