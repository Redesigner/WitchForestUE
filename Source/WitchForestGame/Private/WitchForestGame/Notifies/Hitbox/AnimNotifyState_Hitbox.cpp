// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Notifies/Hitbox/AnimNotifyState_Hitbox.h"

#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

void UAnimNotifyState_Hitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
#if WITH_EDITOR
	if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		//SpawnEditorShape(MeshComp->GetWorld(), MeshComp);
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
void UAnimNotifyState_Hitbox::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp->GetWorld()->WorldType != EWorldType::EditorPreview)
	{
		return;
	}

	switch (HitboxParameters.Shape)
	{
	case Box:
	{
		DrawDebugBox(MeshComp->GetWorld(), HitboxParameters.Position, HitboxParameters.BoxExtents, HitboxParameters.Rotation.Quaternion(), FColor::Red, false, FrameDeltaTime * 2.0f);
		break;
	}
	case Capsule:
	{
		DrawDebugCapsule(MeshComp->GetWorld(), HitboxParameters.Position, HitboxParameters.HalfHeight, HitboxParameters.Radius, HitboxParameters.Rotation.Quaternion(), FColor::Red, false, FrameDeltaTime);
		break;
	}
	case Sphere:
	{
		DrawDebugSphere(MeshComp->GetWorld(), HitboxParameters.Position, HitboxParameters.Radius, 32, FColor::Red, false, FrameDeltaTime);
		break;
	}
	}
}
#endif