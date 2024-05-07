// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/BestiaryViewModel.h"

#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"

void UBestiaryViewModel::BindGameState(AWitchForestGameState* GameState)
{
	GameState->OnRecipeLearned.AddUObject(this, &ThisClass::CreatureDiscovered);
	OwningGameState = GameState;

	CreatureDiscovered();
}

void UBestiaryViewModel::CreatureDiscovered()
{
	if (!OwningGameState.IsValid())
	{
		return;
	}

	const AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(OwningGameState->GetDefaultGameMode());
	if (!GameMode)
	{
		return;
	}

	UBestiaryData* Bestiary = GameMode->GetBestiary();
	if (!Bestiary)
	{
		return;
	}
	KnownBestiaryEntries.Reset();

	for (const FGameplayTag& CreatureTag : OwningGameState->GetDiscoveredCreatures())
	{
		FBestiaryEntry BestiaryEntry;
		if (Bestiary->FindBestiaryForTag(CreatureTag, BestiaryEntry))
		{
			UBestiaryUIData* BestiaryData = NewObject<UBestiaryUIData>(this);
			BestiaryData->BestiaryEntry = BestiaryEntry;
			KnownBestiaryEntries.Add(BestiaryData);
		}
	}

	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::KnownBestiaryEntries);
}
