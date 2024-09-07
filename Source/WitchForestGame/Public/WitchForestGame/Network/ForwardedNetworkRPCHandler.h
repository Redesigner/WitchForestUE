// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ForwardedNetworkRPCHandler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UForwardedNetworkRPCHandler : public UInterface
{
	GENERATED_BODY()
};


class WITCHFORESTGAME_API IForwardedNetworkRPCHandler
{
	GENERATED_BODY()

public:
	virtual void HandleForwardedRPC(int Data) = 0;
};
