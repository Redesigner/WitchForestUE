// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "WitchForestCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UWitchForestCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
	UFUNCTION(Exec)
	void SetImmortal(bool bImmortal);
};
