// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Character/Witch/Witch.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Character/Witch/Components/WitchMovementComponent.h"
#include "WitchForestGame/Character/WitchForestInputComponent.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "WitchForestAbility/Input/WitchForestInputConfig.h"
#include "WitchForestAbility/WitchForestASC.h"


AWitch::AWitch(const FObjectInitializer& ObjectInitializer) :
	ACharacter(ObjectInitializer.SetDefaultSubobjectClass(ACharacter::CharacterMovementComponentName, UWitchMovementComponent::StaticClass()))
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AWitch::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(WalkAction, ETriggerEvent::Triggered, this, &AWitch::Move);
	BindActions(PlayerInputComponent);
}

void AWitch::BindActions(UInputComponent* PlayerInputComponent)
{
	if (!InputConfig)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to bind actions to input component on pawn {Name}. InputConfig is not set.", GetName());
		return;
	}
	UWitchForestInputComponent* WitchForestInputComponent = Cast<UWitchForestInputComponent>(PlayerInputComponent);
	if (!WitchForestInputComponent)
	{
		return;
	}
	TArray<uint32> BindHandles;
	WitchForestInputComponent->BindAbilityActions(InputConfig, this, &AWitch::Input_AbilityInputTagPressed, &AWitch::Input_AbilityInputTagReleased, BindHandles);
}

void AWitch::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	UWitchForestASC* WitchForestASC = GetWitchForestASC();
	if (!WitchForestASC)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to activate ability input in AWitch {Name}. Unable to access AbilitySystemComponent from player state.", ("Name", GetFName()));
		return;
	}
	WitchForestASC->AbilityInputTagPressed(InputTag);
}

void AWitch::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	UWitchForestASC* WitchForestASC = GetWitchForestASC();
	if (!WitchForestASC)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to activate ability input in AWitch {Name}. Unable to access AbilitySystemComponent from player state.", ("Name", GetFName()));
		return;
	}
	WitchForestASC->AbilityInputTagReleased(InputTag);
}

void AWitch::Move(const FInputActionInstance& Instance)
{
	FVector2D Input = Instance.GetValue().Get<FVector2D>();
	if (Controller && !Input.IsNearlyZero())
	{
		//const float Rotation = Controller->GetControlRotation().Yaw;
		const float Rotation = FollowCamera->GetComponentRotation().Yaw + 90.0f;
		const FVector2D RotatedInput = Input.GetRotated(-Rotation);
		const FVector Movement = FVector(RotatedInput.X, -RotatedInput.Y, 0.0);
		AddMovementInput(Movement);
	}
}

UWitchForestASC* AWitch::GetWitchForestASC() const
{
	APlayerState* State = GetPlayerState();
	if (!State)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to activate ability input in AWitch {Name}. The player state is invalid.", GetName());
		return nullptr;
	}
	AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(State);
	if (!WitchPlayerState)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to activate ability input in AWitch {Name}. The player state is an unexpected type. It must be of type AWitchPlayerState.", GetName());
		return nullptr;
	}
	return WitchPlayerState->GetWitchForestASC();
}
