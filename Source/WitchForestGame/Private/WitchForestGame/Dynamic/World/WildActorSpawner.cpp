// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/WildActorSpawner.h"

#include "Components/BoxComponent.h"

AWildActorSpawner::AWildActorSpawner()
{
	SpawnRegion = CreateDefaultSubobject<UBoxComponent>("SpawnRegion");

	// The client doesn't need to use this actor, since it will always replicate spawns
	bNetLoadOnClient = false;
}

void AWildActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bActive)
	{
		SpawnActor();
	}
}

void AWildActorSpawner::SpawnActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!SpawnClass)
	{
		return;
	}
	
	AActor* SpawnedActor = World->SpawnActor<AActor>(SpawnClass, GetRandomSpawnPosition(), FRotator());
	if (!SpawnedActor)
	{
		return;
	}

	SpawnedActor->OnDestroyed.AddUniqueDynamic(this, &ThisClass::SpawnedActorDestroyed);
	CurrentlySpawnedAmount++;

	if (CurrentlySpawnedAmount < MaxSpawnAmount)
	{
		World->GetTimerManager().SetTimer(RespawnTimer, FTimerDelegate::CreateUObject(this, &ThisClass::SpawnActor), SpawnDelay, false, -1.0f);
	}
}

FVector AWildActorSpawner::GetRandomSpawnPosition() const
{
	if (!SpawnRegion)
	{
		return FVector();
	}

	FVector SpawnRegionSize = SpawnRegion->GetUnscaledBoxExtent() / 2.0f;
	FVector SpawnLocation = FVector(
		FMath::RandRange(-SpawnRegionSize.X, SpawnRegionSize.X),
		FMath::RandRange(-SpawnRegionSize.Y, SpawnRegionSize.Y),
		FMath::RandRange(-SpawnRegionSize.Z, SpawnRegionSize.Z)
		);
	return GetActorTransform().TransformPosition(SpawnLocation);
}

void AWildActorSpawner::SpawnedActorDestroyed(AActor* SpawnedActor)
{
	CurrentlySpawnedAmount--;

	if (GetWorld() && !GetWorld()->GetTimerManager().IsTimerActive(RespawnTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, FTimerDelegate::CreateUObject(this, &ThisClass::SpawnActor), SpawnDelay, false, -1.0f);
	}
}
