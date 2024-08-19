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

	UFUNCTION(Exec)
	void KillPlayer();

	UFUNCTION(Exec)
	void EndDay();

	UFUNCTION(Exec)
	void SpawnItem(FString ItemName, uint8 Count = 1);

	UFUNCTION(Exec)
	void RegenerateCurse();
};
