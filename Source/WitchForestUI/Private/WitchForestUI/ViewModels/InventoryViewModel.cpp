// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/InventoryViewModel.h"

#include "WitchForestUI.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"
#include "WitchForestGame/Inventory/ItemSet.h"

#include "Logging/StructuredLog.h"


void UInventoryViewModel::BindInventory(UInventoryComponent* Inventory, UItemSet* ItemSet, uint8 Index)
{
	if (!Inventory)
	{
		UE_LOGFMT(LogWitchForestUI, Error, "InventoryViewModel '{ModelName}' failed to bind Inventory. Inventory was invalid.", GetName());
		return;
	}

	if (!ItemSet)
	{
		UE_LOGFMT(LogWitchForestUI, Error, "InventoryViewModel '{ModelName}' failed to bind Inventory. ItemSet was invalid.", GetName());
		return;
	}

	RelevantItemSet = ItemSet;
	SlotIndex = Index;
	SlotTag = Inventory->GetItemBySlot(Index);

	FInventoryItemData InventoryItemData;
	if (RelevantItemSet->FindItemDataForTag(SlotTag, InventoryItemData))
	{
		TSoftObjectPtr<UMaterialInterface> ItemIcon = InventoryItemData.ItemIcon;
		UE_MVVM_SET_PROPERTY_VALUE(DisplayIcon, InventoryItemData.ItemIcon.Get());
		UE_MVVM_SET_PROPERTY_VALUE(DisplayName, FText::FromName(InventoryItemData.ItemName));
	}
	UE_MVVM_SET_PROPERTY_VALUE(bSelected, Inventory->GetSelectedIndex() == Index);

	Inventory->OnSlotChanged.AddUObject(this, &ThisClass::OnInventorySlotChanged);
	Inventory->OnSelectedIndexChanged.AddUObject(this, &ThisClass::OnSelectedIndexChanged);
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

void UInventoryViewModel::OnSelectedIndexChanged(uint8 Index)
{
	UE_MVVM_SET_PROPERTY_VALUE(bSelected, SlotIndex == Index);
}
