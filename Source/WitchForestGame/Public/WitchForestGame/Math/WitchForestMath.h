// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"


class WITCHFORESTGAME_API WitchForestMath
{
public:
	static FVector MakeLaunchVector(float MaxSpeed, float MinSpeed, float MinPitch, float MaxPitch);
};
