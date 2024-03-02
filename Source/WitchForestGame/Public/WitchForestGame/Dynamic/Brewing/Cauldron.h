// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "GameplayTagContainer.h"

#include "Cauldron.generated.h"


class USphereComponent;
class UPotionRecipeSet;

UCLASS()
class WITCHFORESTGAME_API ACauldron : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> CauldronVolume;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Brewing, meta = (AllowPrivateAccess = true))
	TArray<FGameplayTag> HeldIngredients;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Brewing, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPotionRecipeSet> RecipeBook;

	FTimerHandle CooldownTimerHandle;

	FString ContentString;


	void Interact(AActor* Source) override;

	FVector MakeLaunchVector(float MaxSpeed, float MinSpeed, float MinPitch, float MaxPitch) const;

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
