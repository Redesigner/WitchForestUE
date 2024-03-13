// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/Portal.h"

#include "Components/BoxComponent.h"

#include "WitchForestGame/Character/Witch/Witch.h"

APortal::APortal()
{
	CollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	CollisionVolume->SetupAttachment(RootComponent);
	CollisionVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::VolumeOverlap);
}

void APortal::VolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWitch* Witch = Cast<AWitch>(OtherActor);
	if (!Witch)
	{
		return;
	}

	Witch->TeleportTo(Destination, Witch->GetActorRotation());
}
