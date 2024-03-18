// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/Flammable.h"

#include "WitchForestGame/WitchForestGameplayTags.h"

#include "GameplayTagAssetInterface.h" 

AFlammable::AFlammable()
{
}

void AFlammable::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bBurning)
	{
		return;
	}

	if (IGameplayTagAssetInterface* TaggedActor = Cast<IGameplayTagAssetInterface>(OtherActor))
	{
		if (TaggedActor->HasMatchingGameplayTag(WitchForestGameplayTags::Element_Fire))
		{
			bBurning = true;
			StartBurning();
		}
	}
}

void AFlammable::FinishBurning()
{
	Destroy();
}
