// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/LockedDoor.h"

#include "Components/BoxComponent.h"
#include "NavAreas/NavArea_Default.h"
#include "NavAreas/NavArea_Null.h"

ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	DoorShape = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorShape"));
	RootComponent = DoorShape;
}

void ALockedDoor::Interact_Implementation(AActor* Source)
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

bool ALockedDoor::CanInteract_Implementation(AActor* Source) const
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
	DoorShape->SetAreaClassOverride(UNavArea_Default::StaticClass());
	OnOpened();
}

void ALockedDoor::Close()
{
	bOpen = false;
	DoorShape->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorShape->SetAreaClassOverride(UNavArea_Null::StaticClass());
	OnClosed();
}
