// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestUIBlueprintLibrary.h"

FVector2D UWitchForestUIBlueprintLibrary::SelectVector2D(bool bSelector, FVector2D TrueVector, FVector2D FalseVector)
{
	return bSelector ? TrueVector : FalseVector;
}

FText UWitchForestUIBlueprintLibrary::SecondsToText(float Seconds)
{
	int SecondsWhole = Seconds;
	if (SecondsWhole > 0)
	{
		int MinutePart = SecondsWhole / 60;
		int SecondPart = SecondsWhole - (MinutePart * 60);

		if (SecondPart >= 10.0f)
		{
			return FText::FromString(FString::Printf(TEXT("%d:%d"), MinutePart, SecondPart));
		}

		return FText::FromString(FString::Printf(TEXT("%d:0%d"), MinutePart, SecondPart));
	}
	return FText::FromString(TEXT("0:00"));
}
