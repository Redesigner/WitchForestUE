// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/LockedDoor.h"

#include "Components/BoxComponent.h"

ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	DoorShape = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorShape"));
	RootComponent = DoorShape;
}

void ALockedDoor::Interact(AActor* Source)
{
	if (bLocked)
	{
		return;
	}

	if (bOpen)
	{
		Close();
	}
	else
	{
		Open();
	}
}

bool ALockedDoor::CanInteract(AActor* Source) const
{
	return !bLocked;
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

	bOpen = true;
	DoorShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnOpened();
}

void ALockedDoor::Close()
{
	bOpen = false;
	DoorShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OnClosed();
}
