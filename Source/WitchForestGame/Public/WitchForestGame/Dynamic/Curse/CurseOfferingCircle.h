// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WitchForestGame/Dynamic/Brewing/Cauldron.h"

#include "CurseOfferingCircle.generated.h"

UCLASS()
class WITCHFORESTGAME_API ACurseOfferingCircle : public ACauldron
{
	GENERATED_BODY()

	void Interact_Implementation(AActor* Source) override;
};
