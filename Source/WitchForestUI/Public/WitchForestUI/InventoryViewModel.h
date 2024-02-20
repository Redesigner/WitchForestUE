// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "GameplayTagContainer.h"

#include "InventoryViewModel.generated.h"

class UItemSet;
class UMaterialInterface;
class UInventoryComponent;

UCLASS()
class WITCHFORESTUI_API UInventoryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	// The ItemSet we will pull our item data from
	UPROPERTY()
	TWeakObjectPtr<UItemSet> RelevantItemSet;

	// Unless there is a better way to support array types, each slot needs its own viewmodel
	// The other option I'm considering is giving a viewmodel a copy of all relevant properties for a number of slots
	// This also isn't ideal, because changing the number of slots means editing the header file

	// Item's preview icon
	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> DisplayIcon;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	bool bSelected = false;

	uint8 SlotIndex = 0;

	// Store the tag so we can check if it changed before trying to pull the relevant data from the ItemSet
	FGameplayTag SlotTag;

public:
	UFUNCTION(BlueprintCallable)
	void BindInventory(UInventoryComponent* Inventory, UItemSet* ItemSet, uint8 Index);

	void OnInventorySlotChanged(FGameplayTag NewTag, uint8 Index);

	void OnSelectedIndexChanged(uint8 Index);
};
