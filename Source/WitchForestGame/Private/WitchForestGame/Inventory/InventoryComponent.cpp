// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "WitchForestGame/Inventory/ItemSet.h"

#include "Net/UnrealNetwork.h"


UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventorySlots)
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

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


uint8 UInventoryComponent::GetSelectedIndex() const
{
	return SelectedIndex;
}


void UInventoryComponent::SetSelectedIndex(uint8 Value)
{
	if (SelectedIndex == Value || Value >= InventorySlots.Num())
	{
		return;
	}
	SelectedIndex = Value;
	OnSelectedIndexChanged.Broadcast(Value);
}

void UInventoryComponent::ServerSetSelectedIndex_Implementation(uint8 Value)
{
	if (Value < InventorySlots.Num())
	{
		SelectedIndex = Value;
	}
}


void UInventoryComponent::ShiftUp()
{
	if (SelectedIndex == InventorySlots.Num() - 1)
	{
		SelectedIndex = 0;
	}
	else
	{
		SelectedIndex++;
	}
	OnSelectedIndexChanged.Broadcast(SelectedIndex);
	ServerSetSelectedIndex(SelectedIndex);
}


void UInventoryComponent::ShiftDown()
{
	if (SelectedIndex == 0)
	{
		SelectedIndex = InventorySlots.Num() - 1;
	}
	else
	{
		SelectedIndex--;
	}
	OnSelectedIndexChanged.Broadcast(SelectedIndex);
	ServerSetSelectedIndex(SelectedIndex);
}


void UInventoryComponent::DropItems()
{
	for (int i = 0; i < InventorySlots.Num(); ++i)
	{
		FGameplayTag& Tag = InventorySlots[i];
		Tag = TAG_ItemEmpty;
		OnSlotChanged.Broadcast(Tag, i);
	}
}


void UInventoryComponent::OnRep_Inventory(const TArray<FGameplayTag>& OldInventory)
{
	for (int i = 0; i < InventorySlots.Num(); ++i)
	{
		if (OldInventory[i] != InventorySlots[i])
		{
			OnSlotChanged.Broadcast(InventorySlots[i], i);
		}
	}
}
