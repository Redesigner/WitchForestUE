// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WitchForestUIBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTUI_API UWitchForestUIBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category = "WitchForestUI", meta = (DisplayName = "To Bool (Vector2D)"))
	static FVector2D SelectVector2D(bool bSelector, FVector2D TrueVector, FVector2D FalseVector);
};
