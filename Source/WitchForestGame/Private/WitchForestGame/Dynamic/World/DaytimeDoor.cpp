// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/DaytimeDoor.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "WitchForestGame/Game/WitchForestGameMode.h"

ADaytimeDoor::ADaytimeDoor()
{
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	RootComponent = InteractionVolume;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
}

void ADaytimeDoor::DayEnd()
{
	Super::DayEnd();

	DoorMesh->SetVisibility(true, true);
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADaytimeDoor::DayStart()
{
	Super::DayStart();

	DoorMesh->SetVisibility(false, true);
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADaytimeDoor::Interact_Implementation(AActor* Source)
{
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	GameMode->RequestStartDay();
}
