// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/ActorPreviewComponent.h"

void UActorPreviewComponent::SetPreviewClass(const TSubclassOf<AActor>& PreviewClass)
{
	PreviewActorClass = PreviewClass;
	SpawnPreviewActor();
}

#if WITH_EDITOR
void UActorPreviewComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(UActorPreviewComponent, PreviewActorClass))
	{
		SpawnPreviewActor();
	}
}
#endif

void UActorPreviewComponent::SpawnPreviewActor()
{
	if (!PreviewActorClass)
	{
		if (PreviewActor)
		{
			PreviewActor->Destroy();
			PreviewActor = nullptr;
		}
		return;
	}
		
	if (PreviewActor)
	{
		if (PreviewActor->GetClass() == PreviewActorClass.Get())
		{
			return;
		}
		PreviewActor->Destroy();
	}

	PreviewActor = GetWorld()->SpawnActor(PreviewActorClass.Get());
	PreviewActor->SetActorTickEnabled(false);
	PreviewActor->SetActorEnableCollision(false);
	PreviewActor->DisableComponentsSimulatePhysics();
	PreviewActor->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
}
