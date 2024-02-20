// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "ItemTag", AllowPrivateAccess = true))
	TArray<FGameplayTag> InventorySlots;

public:	
	UInventoryComponent();

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	FGameplayTag GetItemBySlot(uint8 SlotIndex) const;

	bool SetItemBySlot(uint8 SlotIndex, FGameplayTag NewValue);

	bool HasItem(FGameplayTag ItemToFind) const;

	FGameplayTag ConsumeItem(uint8 SlotIndex);

	TArray<FGameplayTag> GetItemSlots() const;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlotChanged, FGameplayTag, uint8)
	FOnSlotChanged OnSlotChanged;
};
