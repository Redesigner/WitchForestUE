// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorParameterWidgetInterface.generated.h"


UINTERFACE(MinimalAPI)
class UActorParameterWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class WITCHFORESTGAME_API IActorParameterWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddActorParameter(AActor* Parameter);
};
