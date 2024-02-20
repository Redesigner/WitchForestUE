// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "WitchForestGame/Inventory/ItemSet.h"

UInventoryComponent::UInventoryComponent()
{
}

#if WITH_EDITOR
void UInventoryComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, InventorySlots))
	{
		for (FGameplayTag& GameplayTag : InventorySlots)
		{
			if (GameplayTag == FGameplayTag::EmptyTag)
			{
				// Force our tags to be the custom native tag, rather than simply the built-in empty tag
				// Mostly for safety and visibility, as EmptyTag seems to be for invalid or unset states
				GameplayTag = TAG_ItemEmpty;
			}
		}
	}
}
#endif


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
	if (InventorySlots[SlotIndex].MatchesTagExact(TAG_ItemEmpty))
	{
		InventorySlots[SlotIndex] = NewValue;

		OnSlotChanged.Broadcast(NewValue, SlotIndex);

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
		return TAG_ItemEmpty;
	}
	FGameplayTag ConsumedItem = InventorySlots[SlotIndex];
	InventorySlots[SlotIndex] = TAG_ItemEmpty;

	OnSlotChanged.Broadcast(TAG_ItemEmpty, SlotIndex);

	return ConsumedItem;
}


TArray<FGameplayTag> UInventoryComponent::GetItemSlots() const
{
	return InventorySlots;
}
