// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Character/Witch/Witch.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Character/Witch/Components/WitchMovementComponent.h"
#include "WitchForestGame/Character/WitchForestInputComponent.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Game/WitchForestGameplayTags.h"

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

	ItemHandle = CreateDefaultSubobject<UItemHandleComponent>(TEXT("ItemHandle"));
	ItemHandle->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::InteractVolumeBeginOverlap);
	InteractionVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::InteractVolumeEndOverlap);
}

void AWitch::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetActorLocation().Z <= -200.0f)
	{
		TeleportToStart();
	}
}

void AWitch::InteractVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(OtherActor))
	{
		OnPotentialInteractionsChanged.Broadcast();
		return;
	}

	if (APickup* Pickup = Cast<APickup>(OtherActor))
	{
		OnPotentialInteractionsChanged.Broadcast();
		return;
	}
}

void AWitch::InteractVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(OtherActor))
	{
		OnPotentialInteractionsChanged.Broadcast();
		return;
	}

	if (APickup* Pickup = Cast<APickup>(OtherActor))
	{
		OnPotentialInteractionsChanged.Broadcast();
		return;
	}
}

void AWitch::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// Input->BindAction(WalkAction, ETriggerEvent::Triggered, this, &AWitch::Move);
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

	WitchForestInputComponent->BindNativeAction(InputConfig, WitchForestGameplayTags::InputTag_Move,
		ETriggerEvent::Triggered, this, &ThisClass::Move, /*bLogIfNotFound=*/ false);

	WitchForestInputComponent->BindNativeAction(InputConfig, WitchForestGameplayTags::InputTag_ShiftSlot,
		ETriggerEvent::Triggered, this, &ThisClass::ShiftSlot, /*bLogIfNotFound=*/ false);
}

void AWitch::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AWitchPlayerState* WitchPlayerState = NewController->GetPlayerState<AWitchPlayerState>();
	if (!WitchPlayerState)
	{
		return;
	}

	UWitchForestASC* ASC = WitchPlayerState->GetWitchForestASC();
	if (!ASC)
	{
		return;
	}

	ASC->SetAvatarActor(this);
	
	if (UWitchMovementComponent* WitchMovementComponent = Cast<UWitchMovementComponent>(GetMovementComponent()))
	{
		WitchMovementComponent->SetASC(ASC);
	}
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

void AWitch::ShiftSlot(const FInputActionInstance& Instance)
{
	APlayerState* MyPlayerState = GetPlayerState();
	if (!MyPlayerState)
	{
		return;
	}

	UInventoryComponent* InventoryComponent = MyPlayerState->GetComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		return;
	}

	float Input = Instance.GetValue().GetMagnitude();
	if (FMath::Abs(Input) <= 0.9f)
	{
		return;
	}

	if (Input > 0.0f)
	{
		InventoryComponent->ShiftUp();
	}
	else
	{
		InventoryComponent->ShiftDown();
	}
}

USphereComponent* AWitch::GetInteractionVolume() const
{
	return InteractionVolume;
}

UAbilitySystemComponent* AWitch::GetAbilitySystemComponent() const
{
	return GetWitchForestASC();
}

UWitchForestASC* AWitch::GetWitchForestASC() const
{
	APlayerState* State = GetPlayerState();
	if (!State)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to get ASC in AWitch {Name}. The player state is invalid.", GetName());
		return nullptr;
	}
	AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(State);
	if (!WitchPlayerState)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to get ASC in AWitch {Name}. The player state is an unexpected type. It must be of type AWitchPlayerState.", GetName());
		return nullptr;
	}
	return WitchPlayerState->GetWitchForestASC();
}

void AWitch::TeleportToStart()
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (!GameModeBase)
	{
		return;
	}

	AController* MyPlayerController = GetController();
	if (!MyPlayerController)
	{
		return;
	}

	if (AActor* StartActor = GameModeBase->FindPlayerStart(MyPlayerController))
	{
		TeleportTo(StartActor->GetActorLocation(), GetActorRotation());
	}
}
