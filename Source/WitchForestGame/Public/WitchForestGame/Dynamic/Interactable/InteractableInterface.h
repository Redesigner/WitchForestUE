// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"


UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};


// Every game needs one!
class WITCHFORESTGAME_API IInteractableInterface
{
	GENERATED_BODY()

public:
	virtual void Interact(AActor* Source) = 0;
};
