// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Math/WitchForestMath.h"

FVector WitchForestMath::MakeLaunchVector(float MaxSpeed, float MinSpeed, float MinPitch, float MaxPitch)
{
	const float LaunchPitch = FMath::FRandRange(MinPitch, MaxPitch);
	const float LaunchYaw = FMath::FRandRange(-PI, PI);
	const float LaunchSpeed = FMath::FRandRange(MinSpeed, MaxSpeed);
	const float CircleRadius = FMath::Sin(LaunchPitch);
	return FVector(
		FMath::Cos(LaunchYaw) * CircleRadius,
		FMath::Sin(LaunchYaw) * CircleRadius,
		FMath::Cos(LaunchPitch)) * LaunchSpeed;
}
