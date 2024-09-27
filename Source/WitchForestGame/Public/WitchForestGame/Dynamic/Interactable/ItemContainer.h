// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"
#include "WitchForestGame/Network/ForwardedNetworkRPCHandler.h"

#include "GameplayTagContainer.h"

#include "ItemContainer.generated.h"

class UArrowComponent;
class APickup;
class UGameplayEffect;

UCLASS()
class WITCHFORESTGAME_API AItemContainer : public AActor, public IInteractableInterface, public IForwardedNetworkRPCHandler
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnContentsChanged, const TArray<FGameplayTag>&)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_Items, meta = (AllowPrivateAccess = true, Categories = ItemTag))
	TArray<FGameplayTag> Items;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Display, meta = (AllowPrivateAccess = true))
	//TSubclassOf<UUserWidget> ContainerWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Widget, meta = (AllowPrivateAccess = true, Categories = "UI.MessageTag"))
	FGameplayTag WidgetMessageTag;
	
	// The effect we apply to the interacting actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Interaction, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> InteractionEffect;


	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UArrowComponent> LaunchVectorArrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Launching, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float MaxLaunchVelocity = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Launching, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float MinLaunchVelocity = 25.0f;

	// The max θ of our launch angle 'cone', in degrees
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Launching, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float LaunchAngleRandomArc = 20.0f;

public:
	FOnContentsChanged OnContentsChanged;

	const TArray<FGameplayTag>& GetItems() const { return Items; }

	void RequestLaunchItemByIndex(APlayerController* Requester, int Index);

private:
	AItemContainer();

	void BeginPlay() override;

	void Interact_Implementation(AActor* Source) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// This function is mostly copied from Cauldron... maybe this should be shared somewhere?
	void LaunchItem(TSubclassOf<APickup> Item);

	void LaunchItemByIndex(int Index);

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	uint8 CalculateEmptySlots() const;

	bool HasEmptySlot(int& AvailableSlotIndexOut) const;
	
	FVector MakeLaunchVector() const;

	UFUNCTION()
	void OnRep_Items(TArray<FGameplayTag> OldTags);

	void HandleForwardedRPC(int Data) override;

	uint8 EmptySlotCount = 0;
};
