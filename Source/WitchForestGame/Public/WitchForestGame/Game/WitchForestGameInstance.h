// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Engine/StreamableManager.h" 

#include "WitchForestGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UWitchForestGameInstance : public UGameInstance
{
	GENERATED_BODY()

	FStreamableManager StreamableManager;

public:
	FStreamableManager& GetStreamableManager();
};
