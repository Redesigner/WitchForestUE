// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/GrowableSprout.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Witch/Witch.h"

#include "GameplayTagAssetInterface.h" 
#include "Components/BoxComponent.h"
#include "GameFramework/PawnMovementComponent.h" 

const FName PlantHideTag = "HideWhenAlive";
const FName PlantShowTag = "ShowWhenAlive";

AGrowableSprout::AGrowableSprout()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	PlatformStartLocation = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformStart"));
	PlatformStartLocation->SetupAttachment(RootComponent);

	PlatformEndLocation = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformEnd"));
	PlatformEndLocation->SetupAttachment(RootComponent);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	PlatformMesh->SetupAttachment(RootComponent);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(PlatformMesh);
	TriggerVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::ObjectEnterTrigger);
}

void AGrowableSprout::Tick(float Delta)
{
	Super::Tick(Delta);

	if (!bMoving)
	{
		return;
	}

	CurrentMoveTime += bExtended ? -Delta : Delta;
	const float MoveAlpha = CurrentMoveTime / MoveTime;
	const FVector CurrentLocation = PlatformMesh->GetComponentLocation();
	const FVector EndLocation = PlatformEndLocation->GetComponentLocation();

	// If we're currently extending, and we've extended past our max
	if (!bExtended && CurrentMoveTime >= MoveTime)
	{
		CurrentMoveTime = MoveTime;
		bMoving = false;
		bExtended = true;
	}
	// We're currently retracting, and can't retract further
	if (bExtended && CurrentMoveTime <= 0.0f)
	{
		CurrentMoveTime = 0.0f;
		bMoving = false;
		bExtended = false;
	}

	const FVector DesiredLocation = FMath::Lerp(PlatformStartLocation->GetComponentLocation(), EndLocation, MoveAlpha);
	PlatformMesh->MoveComponent(DesiredLocation - CurrentLocation, PlatformMesh->GetComponentRotation(), false);
}

void AGrowableSprout::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bAlive)
	{
		return;
	}

	if (IGameplayTagAssetInterface* TaggedActor = Cast<IGameplayTagAssetInterface>(OtherActor))
	{
		if (TaggedActor->HasMatchingGameplayTag(WitchForestGameplayTags::Element_Plant))
		{
			OtherActor->Destroy();
			Grow();
		}
	}
}

void AGrowableSprout::Grow()
{
	bAlive = true;

	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	for (UActorComponent* ActorComponent : GetComponents())
	{
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(ActorComponent))
		{
			if (Primitive->ComponentHasTag(PlantHideTag))
			{
				Primitive->SetVisibility(false);
			}
			else if (Primitive->ComponentHasTag(PlantShowTag))
			{
				Primitive->SetVisibility(true);
			}
		}
	}
}

void AGrowableSprout::ObjectEnterTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bMoving || !bAlive)
	{
		return;
	}

	AWitch* Witch = Cast<AWitch>(OtherActor);
	if (!Witch)
	{
		return;
	}

	UPawnMovementComponent* PawnMovement = Witch->GetMovementComponent();
	if (!PawnMovement)
	{
		return;
	}

	bMoving = true;
}
