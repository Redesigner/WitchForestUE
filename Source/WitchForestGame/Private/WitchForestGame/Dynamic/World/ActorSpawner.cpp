// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/ActorSpawner.h"

#include "Components/BoxComponent.h"

AActorSpawner::AActorSpawner()
{
	SpawnRegion = CreateDefaultSubobject<UBoxComponent>("SpawnRegion");
}

void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnActor();
}

void AActorSpawner::SpawnActor()
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

FVector AActorSpawner::GetRandomSpawnPosition() const
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

void AActorSpawner::SpawnedActorDestroyed(AActor* SpawnedActor)
{
	CurrentlySpawnedAmount--;

	if (GetWorld() && !GetWorld()->GetTimerManager().IsTimerActive(RespawnTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, FTimerDelegate::CreateUObject(this, &ThisClass::SpawnActor), SpawnDelay, false, -1.0f);
	}
}
