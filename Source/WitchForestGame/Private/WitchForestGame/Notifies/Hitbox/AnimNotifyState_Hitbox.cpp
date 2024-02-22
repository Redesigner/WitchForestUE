// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Notifies/Hitbox/AnimNotifyState_Hitbox.h"

#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

void UAnimNotifyState_Hitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		SpawnEditorShape();
		return;
	}
#endif
	AActor* OwnerActor = MeshComp->GetOwner();
	UMeleeComponent* MeleeComponent = OwnerActor->GetComponentByClass<UMeleeComponent>();
	if (!MeleeComponent)
	{
		return;
	}
	Hitbox = MeleeComponent->SpawnHitbox(HitboxParameters);
}

void UAnimNotifyState_Hitbox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		if (HitboxPreviewShape)
		{
			HitboxPreviewShape->DestroyComponent();
		}
		return;
	}
#endif
	AActor* OwnerActor = MeshComp->GetOwner();
	UMeleeComponent* MeleeComponent = OwnerActor->GetComponentByClass<UMeleeComponent>();
	if (!MeleeComponent)
	{
		return;
	}
	MeleeComponent->DestroyHitbox(Hitbox.Get());
}

void UAnimNotifyState_Hitbox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
}

#if WITH_EDITOR
void UAnimNotifyState_Hitbox::SpawnEditorShape()
{
	UShapeComponent* SpawnedShape = nullptr;
	switch (HitboxParameters.Shape)
	{
		case Box:
		{
			UBoxComponent* Box = NewObject<UBoxComponent>();
			Box->SetBoxExtent(HitboxParameters.BoxExtents);
			SpawnedShape = Box;
			break;
		}
		case Capsule:
		{
			UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>();
			Capsule->SetCapsuleRadius(HitboxParameters.Radius);
			Capsule->SetCapsuleHalfHeight(HitboxParameters.HalfHeight);
			SpawnedShape = Capsule;
			break;
		}
		case Sphere:
		{
			USphereComponent* Sphere = NewObject<USphereComponent>();
			Sphere->SetSphereRadius(HitboxParameters.Radius);
			SpawnedShape = Sphere;
			break;
		}
	}
	SpawnedShape->SetWorldLocationAndRotation(HitboxParameters.Position, HitboxParameters.Rotation);
	SpawnedShape->RegisterComponentWithWorld(GetWorld());
}
#endif