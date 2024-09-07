// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WitchForestGame/Network/ForwardedNetworkRPCHandler.h"

#include "ForwardNetworkRPCComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTGAME_API UForwardNetworkRPCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void ServerForwardRequest(AActor* Handler, int32 Data);
};
