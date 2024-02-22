// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Components/MeleeComponent.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

UMeleeComponent::UMeleeComponent()
{
}


UShapeComponent* UMeleeComponent::SpawnHitbox(FMeleeHitboxSpawnParameters SpawnParameters)
{
	USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
	if (!SkeletalMesh)
	{
		return nullptr;
	}
	UShapeComponent* SpawnedShape = nullptr;
	switch (SpawnParameters.Shape)
	{
	case Box:
		{
			UBoxComponent* Box = NewObject<UBoxComponent>();
			Box->SetBoxExtent(SpawnParameters.BoxExtents);
			SpawnedShape = Box;
			break;
		}
	case Capsule:
		{
			UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>();
			Capsule->SetCapsuleRadius(SpawnParameters.Radius);
			Capsule->SetCapsuleHalfHeight(SpawnParameters.HalfHeight);
			SpawnedShape = Capsule;
			break;
		}
	case Sphere:
		{
			USphereComponent* Sphere = NewObject<USphereComponent>();
			Sphere->SetSphereRadius(SpawnParameters.Radius);
			SpawnedShape = Sphere;
			break;
		}
	}
	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	SpawnedShape->AttachToComponent(SkeletalMesh, AttachmentTransformRules);
	SpawnedShape->SetRelativeLocationAndRotation(SpawnParameters.Position, SpawnParameters.Rotation);
	SpawnedShape->SetCollisionProfileName(CollisionProfile.Name);
	SpawnedShape->IgnoreActorWhenMoving(GetOwner(), true);
	SpawnedShape->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnHitboxOverlap);
	SpawnedShape->RegisterComponent();
	SpawnedShape->UpdateOverlaps();
	SpawnedShape->SetHiddenInGame(false);
	SpawnedShape->SetVisibility(bShowHitboxes);
	SpawnedHitboxes.AddUnique(SpawnedShape);
	return SpawnedShape;
}

void UMeleeComponent::DestroyHitbox(UShapeComponent* Hitbox)
{
	int32 ShapeIndex;
	if (SpawnedHitboxes.Find(Hitbox, ShapeIndex))
	{
		SpawnedHitboxes.RemoveAt(ShapeIndex);
		Hitbox->DestroyComponent();
	}
}

void UMeleeComponent::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnActorHit.Broadcast(OtherActor);
}
