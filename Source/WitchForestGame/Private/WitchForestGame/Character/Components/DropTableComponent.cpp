// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Components/DropTableComponent.h"

#include "WitchForestGame/Inventory/DropTable.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Dynamic/Pickup.h"

#include "Kismet/GameplayStatics.h"

UDropTableComponent::UDropTableComponent()
{
}

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
			NewPickup->AddImpulse(GetLaunchVector());
		}
	}
}

FVector UDropTableComponent::GetLaunchVector() const
{
	const float LaunchPitch = FMath::FRandRange(PI / 4.0f, PI / 2.0f);
	const float LaunchYaw = FMath::FRandRange(-PI, PI);
	const float LaunchSpeed = FMath::FRandRange(5000.0f, 20000.0f);
	const float CircleRadius = FMath::Cos(LaunchPitch);
	return FVector(
		FMath::Cos(LaunchYaw) * CircleRadius,
		FMath::Sin(LaunchYaw) * CircleRadius,
		FMath::Sin(LaunchPitch)) * LaunchSpeed;
}
