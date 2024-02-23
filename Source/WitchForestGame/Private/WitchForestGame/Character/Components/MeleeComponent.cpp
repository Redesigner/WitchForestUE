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
			UBoxComponent* Box = NewObject<UBoxComponent>(this);
			Box->SetBoxExtent(SpawnParameters.BoxExtents);
			SpawnedShape = Box;
			break;
		}
	case Capsule:
		{
			UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(this);
			Capsule->SetCapsuleRadius(SpawnParameters.Radius);
			Capsule->SetCapsuleHalfHeight(SpawnParameters.HalfHeight);
			SpawnedShape = Capsule;
			break;
		}
	case Sphere:
		{
			USphereComponent* Sphere = NewObject<USphereComponent>(this);
			Sphere->SetSphereRadius(SpawnParameters.Radius);
			SpawnedShape = Sphere;
			break;
		}
	}
	SpawnedShape->SetGenerateOverlapEvents(true);
	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	SpawnedShape->AttachToComponent(SkeletalMesh, AttachmentTransformRules);
	SpawnedShape->SetRelativeLocationAndRotation(SpawnParameters.Position, SpawnParameters.Rotation);

	// Set the collision type, but make the object type dynamic
	SpawnedShape->SetCollisionProfileName(CollisionProfile.Name);
	SpawnedShape->SetCollisionObjectType(ECC_WorldDynamic);
	// Even though this ignores moving, it also works for the registration function later
	SpawnedShape->IgnoreActorWhenMoving(GetOwner(), true);
	SpawnedShape->SetHiddenInGame(false);
	SpawnedShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpawnedShape->SetVisibility(bShowHitboxes);

	SpawnedShape->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnHitboxOverlap);

	// Overlaps are automatically updated here
	SpawnedShape->RegisterComponentWithWorld(GetWorld());
	TOverlapArrayView Overlaps;

	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByProfile(OverlapResults, SpawnedShape->GetComponentLocation(), SpawnedShape->GetComponentQuat(), SpawnedShape->GetCollisionProfileName(), SpawnedShape->GetCollisionShape());	UE_LOG(LogTemp, Display, TEXT("%i overlaps found when using world overlap test."), OverlapResults.Num());

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
	UE_LOG(LogTemp, Display, TEXT("MeleeComponent began an overlap."));
	OnActorHit.Broadcast(OtherActor);
}
