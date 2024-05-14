// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/Freezable.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "GameplayTagAssetInterface.h" 
#include "Components/BoxComponent.h"

const FName HideTag = "HideWhenUnfrozen";
const FName ShowTag = "ShowWhenFrozen";
const FName DisableCollisionTag = "DisableCollisionWhenFrozen";
const FName EnableCollisionTag = "EnableCollisionWhenFrozen";

// This shares a lot of code with Flammable, so I could probably reuse it in a more generic way
AFreezable::AFreezable()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	EndZone = CreateDefaultSubobject<UBoxComponent>(TEXT("EndZone"));
	EndZone->SetupAttachment(RootComponent);
	EndZone->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::ObjectEnterEndZone);
}

void AFreezable::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (IGameplayTagAssetInterface* TaggedActor = Cast<IGameplayTagAssetInterface>(OtherActor))
	{
		if (TaggedActor->HasMatchingGameplayTag(WitchForestGameplayTags::Element_Ice))
		{
			if (!TimerManager.IsTimerActive(FreezeTimer))
			{
				Freeze();
			}
			GetWorld()->GetTimerManager().SetTimer(FreezeTimer, FTimerDelegate::CreateUObject(this, &ThisClass::Unfreeze), FreezeTime, false, -1.0f);
		}
	}
}

void AFreezable::Freeze()
{
	for (UActorComponent* ActorComponent : GetComponents())
	{
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(ActorComponent))
		{
			if (Primitive->ComponentHasTag(HideTag))
			{
				Primitive->SetVisibility(false);
			}
			else if (Primitive->ComponentHasTag(ShowTag))
			{
				Primitive->SetVisibility(true);
			}

			if (Primitive->ComponentHasTag(DisableCollisionTag))
			{
				Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else if (Primitive->ComponentHasTag(EnableCollisionTag))
			{
				Primitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}
	}
}

void AFreezable::Unfreeze()
{
	for (UActorComponent* ActorComponent : GetComponents())
	{
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(ActorComponent))
		{
			if (Primitive->ComponentHasTag(HideTag))
			{
				Primitive->SetVisibility(true);
			}
			else if (Primitive->ComponentHasTag(ShowTag))
			{
				Primitive->SetVisibility(false);
			}

			if (Primitive->ComponentHasTag(DisableCollisionTag))
			{
				Primitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
			else if (Primitive->ComponentHasTag(EnableCollisionTag))
			{
				Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

void AFreezable::ObjectEnterEndZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AWitch* Witch = Cast<AWitch>(OtherActor))
	{
		Witch->TeleportToLastSafeLocation();
		return;
	}

	if (APickup* Pickup = Cast<APickup>(OtherActor))
	{
		Pickup->Destroy();
		return;
	}
}
