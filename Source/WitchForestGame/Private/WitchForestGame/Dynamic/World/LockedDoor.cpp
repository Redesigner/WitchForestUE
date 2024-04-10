// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/LockedDoor.h"

#include "Components/BoxComponent.h"

ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	DoorShape = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorShape"));
	RootComponent = DoorShape;
}

void ALockedDoor::Unlock()
{
	bLocked = false;

	OnUnlocked();

	if (bOpenWhenUnlocking)
	{
		Open();
	}
}

void ALockedDoor::Open()
{
	if (bLocked)
	{
		return;
	}

	DoorShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnOpened();
}

void ALockedDoor::Close()
{
	DoorShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
