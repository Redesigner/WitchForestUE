// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "GameplayTagContainer.h"

#include "Cauldron.generated.h"

class USphereComponent;
class UItemSet;
class APickup;

UCLASS()
class WITCHFORESTGAME_API ACauldron : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> CauldronVolume;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Brewing, ReplicatedUsing = OnRep_HeldIngredients, meta = (AllowPrivateAccess = true))
	TArray<FGameplayTag> HeldIngredients;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Brewing, meta = (AllowPrivateAccess = true))
	uint8 Capacity = 4;

	FTimerHandle CooldownTimerHandle;

	FString ContentString;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Interact(AActor* Source) override;

	void LaunchItem(const FGameplayTag& Item, UItemSet* ItemSet);
	void LaunchItem(TSubclassOf<APickup> Item);

	void StartCooldown();

	UFUNCTION()
	void OnRep_HeldIngredients(TArray<FGameplayTag> OldTags);

	void ClearContents();

public:	
	ACauldron();

	UFUNCTION()
	void VolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	FString GetContentsString() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContentsChanged);
	UPROPERTY(BlueprintAssignable)
	FOnContentsChanged OnContentsChanged;
};
