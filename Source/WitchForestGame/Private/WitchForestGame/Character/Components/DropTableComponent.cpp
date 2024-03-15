// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Components/DropTableComponent.h"

#include "WitchForestGame/Inventory/DropTable.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Math/WitchForestMath.h"

#include "Kismet/GameplayStatics.h"


void UDropTableComponent::DropItems()
{
	if (!DropTable)
	{
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode || !GameMode->GetItemSet())
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	UWorld* World = GetWorld();
	AActor* Actor = GetOwner();
	FVector SpawnLocation = Actor->GetActorLocation();
	for (FDropTableAmount DropTableAmount : DropTable->Evaluate())
	{
		FInventoryItemData ItemData;
		if (!ItemSet->FindItemDataForTag(DropTableAmount.ItemTag, ItemData))
		{
			continue;
		}

		for (int i = 0; i < DropTableAmount.Amount; ++i)
		{
			const float RandomYaw = FMath::FRandRange(0.0f, 360.0f);
			const FRotator SpawnRotator = FRotator(0.0f, RandomYaw, 0.0f);
			APickup* NewPickup = World->SpawnActor<APickup>(ItemData.PickupClass, SpawnLocation, SpawnRotator);

			constexpr float MinPitch = FMath::DegreesToRadians(45.0f);
			constexpr float MaxPitch = FMath::DegreesToRadians(85.0f);
			const FVector LaunchVector = WitchForestMath::MakeLaunchVector(20000.0f, 25000.0f, MinPitch, MaxPitch);
			NewPickup->AddImpulse(LaunchVector);
		}
	}
}
