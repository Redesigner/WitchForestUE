// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/InventoryViewModel.h"

#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Inventory/ItemSet.h"


void UInventoryViewModel::BindInventory(UInventoryComponent* Inventory, UItemSet* ItemSet, uint8 Index)
{
	if (!Inventory || !ItemSet)
	{
		return;
	}
	RelevantItemSet = ItemSet;
	SlotIndex = Index;
	SlotTag = Inventory->GetItemBySlot(Index);

	FInventoryItemData InventoryItemData;
	if (RelevantItemSet->FindItemDataForTag(SlotTag, InventoryItemData))
	{
		UE_MVVM_SET_PROPERTY_VALUE(DisplayIcon, InventoryItemData.ItemIcon.Get());
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, FText::FromName(InventoryItemData.ItemName));
	}
	Inventory->OnSlotChanged.AddUObject(this, &ThisClass::OnInventorySlotChanged);
}

void UInventoryViewModel::OnInventorySlotChanged(FGameplayTag NewTag, uint8 Index)
{
	// Don't bother pulling data if our tag hasn't changed
	if (!RelevantItemSet.IsValid() || SlotTag == NewTag || Index != SlotIndex)
	{
		return;
	}
	SlotTag = NewTag;
	if (SlotTag == TAG_ItemEmpty)
	{
		UE_MVVM_SET_PROPERTY_VALUE(DisplayIcon, nullptr);
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, FText());
		return;
	}
	FInventoryItemData InventoryItemData;
	if (RelevantItemSet->FindItemDataForTag(SlotTag, InventoryItemData))
	{
		UE_MVVM_SET_PROPERTY_VALUE(DisplayIcon, InventoryItemData.ItemIcon.Get());
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, FText::FromName(InventoryItemData.ItemName));
	}
}
