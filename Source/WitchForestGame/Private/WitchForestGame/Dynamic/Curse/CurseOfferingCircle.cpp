// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Curse/CurseOfferingCircle.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"

#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameplayMessageSubsystem.h"

void ACurseOfferingCircle::Interact(AActor* Source)
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
		UE_LOGFMT(LogWitchForestGame, Display, "CurseOfferingCircle '{Name}' successfully lifted the curse... for now.", GetName());
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		FWitchForestMessage NewMessage;
		NewMessage.Verb = WitchForestGameplayTags::Event_Discovery_Recipe;
		NewMessage.Data = Recipe.RecipeTag;
		NewMessage.Source = Source;
		MessageSystem.BroadcastMessage(WitchForestGameplayTags::Event_Discovery, NewMessage);
	}
	HeldIngredients.Reset();
	OnContentsChanged.Broadcast();
	// StartCooldown();
}
