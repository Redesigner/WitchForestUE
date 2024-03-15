// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Character/Witch/Components/WitchMovementComponent.h"

UWitchMovementComponent::UWitchMovementComponent()
{
}

void UWitchMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
