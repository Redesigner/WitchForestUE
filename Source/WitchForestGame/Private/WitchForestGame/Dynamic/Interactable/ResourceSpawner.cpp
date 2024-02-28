// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/ResourceSpawner.h"

#include "WitchForestGame/Character/Components/DropTableComponent.h"


AResourceSpawner::AResourceSpawner()
{
	DropTableComponent = CreateDefaultSubobject<UDropTableComponent>(TEXT("DropTable"));
}


void AResourceSpawner::Interact(AActor* Source)
{
	DropTableComponent->DropItems();
}
