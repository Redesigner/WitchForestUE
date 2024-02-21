// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestUIBlueprintLibrary.h"

FVector2D UWitchForestUIBlueprintLibrary::SelectVector2D(bool bSelector, FVector2D TrueVector, FVector2D FalseVector)
{
	return bSelector ? TrueVector : FalseVector;
}
