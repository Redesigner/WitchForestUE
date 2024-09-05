// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "WitchForestGame/Inventory/ItemSet.h"

#include "InventoryItemUIData.generated.h"


// A class holding all the data we need for a recipe, to be exposed to the MVVM panel
// This has to be a class, it can't be a struct
UCLASS(BlueprintType)
class WITCHFORESTUI_API UInventoryItemUIData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FInventoryItemData ItemData;
};