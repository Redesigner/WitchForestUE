// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
}


FGameplayTag UInventoryComponent::GetItemBySlot(uint8 SlotIndex) const
{
	if (SlotIndex >= InventorySlots.Num())
	{
		return FGameplayTag();
	}
	return InventorySlots[SlotIndex];
}


bool UInventoryComponent::SetItemBySlot(uint8 SlotIndex, FGameplayTag NewValue)
{
	if (SlotIndex >= InventorySlots.Num())
	{
		return false;
	}
	if (InventorySlots[SlotIndex].MatchesTagExact(FGameplayTag::EmptyTag))
	{
		InventorySlots[SlotIndex] = NewValue;
		return true;
	}
	return false;
}


bool UInventoryComponent::HasItem(FGameplayTag ItemToFind) const
{
	if (!ItemToFind.IsValid())
	{
		return false;
	}
	for (const FGameplayTag& Item : InventorySlots)
	{
		if (Item.MatchesTagExact(ItemToFind))
		{
			return true;
		}
	}
	return false;
}


FGameplayTag UInventoryComponent::ConsumeItem(uint8 SlotIndex)
{
	if (SlotIndex >= InventorySlots.Num())
	{
		return FGameplayTag::EmptyTag;
	}
	FGameplayTag ConsumedItem = InventorySlots[SlotIndex];
	InventorySlots[SlotIndex] = FGameplayTag::EmptyTag;
	return ConsumedItem;
}
