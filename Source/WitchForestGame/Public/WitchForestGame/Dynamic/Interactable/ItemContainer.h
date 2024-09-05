// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "GameplayTagContainer.h"

#include "ItemContainer.generated.h"

class UArrowComponent;
class APickup;
class UGameplayEffect;

UCLASS()
class WITCHFORESTGAME_API AItemContainer : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnContentsChanged, const TArray<FGameplayTag>&)

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true, Categories = ItemTag))
	TArray<FGameplayTag> Items;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Display, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> ContainerWidgetClass;
	
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

	void LaunchItemByIndex(int Index);

	// This function is mostly copied from Cauldron... maybe this should be shared somewhere?
	void LaunchItem(TSubclassOf<APickup> Item);

private:
	AItemContainer();

	void BeginPlay() override;

	void Interact_Implementation(AActor* Source) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	uint8 CalculateEmptySlots() const;

	bool HasEmptySlot(int& AvailableSlotIndexOut) const;
	
	FVector MakeLaunchVector() const;

	uint8 EmptySlotCount = 0;
};
