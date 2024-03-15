// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/ResourceSpawner.h"

#include "WitchForestGame/Character/Components/DropTableComponent.h"


AResourceSpawner::AResourceSpawner()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	DropTableComponent = CreateDefaultSubobject<UDropTableComponent>(TEXT("DropTable"));
}


void AResourceSpawner::Interact(AActor* Source)
{
	UWorld* World = GetWorld();
	check(World);
	FTimerManager& TimerManager = World->GetTimerManager();

	if (TimerManager.IsTimerActive(CooldownTimerHandle))
	{
		return;
	}

	DropTableComponent->DropItems();

	if (bActivateCooldownOnSpawn)
	{
		StartCooldown();
	}
}

void AResourceSpawner::StartCooldown()
{
	UWorld* World = GetWorld();
	check(World);
	FTimerManager& TimerManager = World->GetTimerManager();

	TimerManager.SetTimer(CooldownTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::EndCooldown), CooldownTime, false);

	StaticMesh->SetVisibility(false, true);
}

void AResourceSpawner::EndCooldown()
{
	StaticMesh->SetVisibility(true, true);
}
