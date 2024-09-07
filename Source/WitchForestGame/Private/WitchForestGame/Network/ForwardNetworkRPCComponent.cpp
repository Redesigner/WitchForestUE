// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Network/ForwardNetworkRPCComponent.h"

void UForwardNetworkRPCComponent::ServerForwardRequest_Implementation(AActor* Handler, int32 Data)
{
	Cast<IForwardedNetworkRPCHandler>(Handler)->HandleForwardedRPC(Data);
}
