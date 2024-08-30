// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Curse/CurseOfferingCircle.h"

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"

#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameplayMessageSubsystem.h"

// @TODO: move this to a separate class instead of inheriting from cauldron and hold our items in game state?
void ACurseOfferingCircle::Interact_Implementation(AActor* Source)
{
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	AWitchForestGameState* GameState = Cast<AWitchForestGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	if (!GameState->TryLiftCurse(HeldIngredients))
	{
		// Launch all of our ingredients back out
		TArray<FGameplayTag> PreviouslyHeldItems = HeldIngredients;
		for (const FGameplayTag& Item : PreviouslyHeldItems)
		{
			LaunchItem(Item, ItemSet);
		}
	}
	else
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		FWitchForestMessage NewMessage;
		NewMessage.Verb = WitchForestGameplayTags::Event_Notification;
		NewMessage.Data = WitchForestGameplayTags::Notification_Curse_Lifted;
		NewMessage.Source = Source;
		MessageSystem.BroadcastMessage(NewMessage.Verb, NewMessage);
	}
	HeldIngredients.Reset();
	OnContentsChanged.Broadcast();
	// StartCooldown();
}

// Reject items that aren't required for lifting our curse?