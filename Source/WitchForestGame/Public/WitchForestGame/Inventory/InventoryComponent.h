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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Inventory, meta = (Categories = "ItemTag", AllowPrivateAccess = true))
	TArray<FGameplayTag> InventorySlots;

	// The currently selected slot
	// This should only be modified on the client side, any inventory changes should pack this value into the server request
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 SelectedIndex = 0;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BeginPlay() override;

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

	// Selected index... should this be moved elsewhere?
	uint8 GetSelectedIndex() const;

	void SetSelectedIndex(uint8 Value);

	UFUNCTION(Server, Reliable)
	void ServerSetSelectedIndex(uint8 Value);

	void ShiftUp();

	void ShiftDown();

	void DropItems(FVector Location);

	UFUNCTION()
	void OnRep_Inventory(const TArray<FGameplayTag>& OldInventory);


	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlotChanged, FGameplayTag, uint8)
	FOnSlotChanged OnSlotChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectedIndexChanged, uint8);
	FOnSelectedIndexChanged OnSelectedIndexChanged;
};
