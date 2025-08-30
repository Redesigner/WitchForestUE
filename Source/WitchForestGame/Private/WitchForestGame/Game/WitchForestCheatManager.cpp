// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestCheatManager.h"

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/WitchPlayerController.h"
#include "WitchForestGame/Character/WitchPlayerState.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Inventory/ItemSet.h"

#include "WitchForestGame/WitchForestASC.h"
#include "WitchForestGame/Attributes/BaseAttributeSet.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

void UWitchForestCheatManager::SetImmortal(bool bImmortal)
{
	AWitchPlayerController* WitchPlayerController = Cast<AWitchPlayerController>(GetOuterAPlayerController());
	if (!WitchPlayerController)
	{
		return;
	}

	AWitchPlayerState* WitchPlayerState = WitchPlayerController->GetPlayerState<AWitchPlayerState>();
	if (!WitchPlayerState)
	{
		return;
	}

	UWitchForestASC* ASC = WitchPlayerState->GetWitchForestASC();
	if (!ASC)
	{
		return;
	}

	if (bImmortal)
	{
		ASC->AddLooseGameplayTag(WitchForestGameplayTags::Cheat_Immortal);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(WitchForestGameplayTags::Cheat_Immortal);
	}
}

void UWitchForestCheatManager::KillPlayer()
{
	AWitchPlayerController* WitchPlayerController = Cast<AWitchPlayerController>(GetOuterAPlayerController());
	if (!WitchPlayerController)
	{
		return;
	}

	AWitchPlayerState* WitchPlayerState = WitchPlayerController->GetPlayerState<AWitchPlayerState>();
	if (!WitchPlayerState)
	{
		return;
	}

	WitchPlayerState->GetAttributeSet()->KillOwner();
}

void UWitchForestCheatManager::EndDay()
{
	if (!GetOuterAPlayerController())
	{
		return;
	}

	if (!GetOuterAPlayerController()->HasAuthority())
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Cannot call cheat 'EndDay'. Player '{PlayerName}' does not have the authority to invoke commands on the server.", GetOuterAPlayerController()->GetName());
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	GameMode->RequestEndDay();
}

void UWitchForestCheatManager::SpawnItem(FString ItemName, uint8 ItemCount)
{
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	UItemSet* CurrentItemSet = GameMode->GetItemSet();
	if (!CurrentItemSet)
	{
		return;
	}

	if (!GetOuterAPlayerController())
	{
		return;
	}

	APawn* Pawn = GetOuterAPlayerController()->GetPawn();
	if (!Pawn)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Player '{PlayerControllerName}' attempted to spawn item using cheat 'SpawnItem', but did not have any pawn to spawn the item with.",
			GetOuterAPlayerController()->GetName());
		return;
	}

	// If our string doesn't start with 'ItemTag.' append it so that testers don't have to type it out every time
	FString ModifiedString;
	if (!ItemName.StartsWith(TEXT("ItemTag."), ESearchCase::CaseSensitive))
	{
		ModifiedString = TEXT("ItemTag.");
		ModifiedString.Append(ItemName);
	}
	else
	{
		ModifiedString = ItemName;
	}

	FInventoryItemData SpawnedItemData;
	if (!CurrentItemSet->FindItemTagFromString(ModifiedString, SpawnedItemData))
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Player '{PlayerControllerName}' attempted to spawn item using cheat 'SpawnItem'. Item '{RequestedItemName}' could not be found in ItemSet '{ItemSetName}'",
			GetOuterAPlayerController()->GetName(), ModifiedString, CurrentItemSet->GetName());
		return;
	}

	for (int i = 0; i < ItemCount; ++i)
	{
		GetWorld()->SpawnActor<AActor>(SpawnedItemData.PickupClass, Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * (150.0f + 50.0f * i), FRotator());
	}
}

void UWitchForestCheatManager::RegenerateCurse()
{
	if (!GetOuterAPlayerController())
	{
		return;
	}

	if (!GetOuterAPlayerController()->HasAuthority())
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Cannot call cheat 'RegenrateCurse'. Player '{PlayerName}' does not have the authority to invoke commands on the server.", GetOuterAPlayerController()->GetName());
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	GameMode->ApplyNewRandomCurse();
}
