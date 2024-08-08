// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestUIBlueprintLibrary.h"

FVector2D UWitchForestUIBlueprintLibrary::SelectVector2D(bool bSelector, FVector2D TrueVector, FVector2D FalseVector)
{
	return bSelector ? TrueVector : FalseVector;
}

FText UWitchForestUIBlueprintLibrary::SecondsToText(float Seconds)
{
	if (Seconds > 0.0f)
	{
		int MinutePart = FMath::Floor(Seconds / 60.f);
		int SecondPart = FMath::RoundToFloat(Seconds - MinutePart);

		if (SecondPart >= 10.0f)
		{
			return FText::FromString(FString::Printf(TEXT("%d:%d"), MinutePart, SecondPart));
		}

		return FText::FromString(FString::Printf(TEXT("%d:0%d"), MinutePart, SecondPart));
	}
	return FText::FromString(TEXT("0:00"));
}
