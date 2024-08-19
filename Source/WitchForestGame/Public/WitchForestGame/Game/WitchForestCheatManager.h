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

	/// Kill the currently controlled player
	UFUNCTION(Exec)
	void KillPlayer();

	/// Force the day to end, if it is still daytime
	UFUNCTION(Exec)
	void EndDay();

	/// Spawn items in front of the currently controlled player
	UFUNCTION(Exec)
	void SpawnItem(FString ItemName, uint8 Count = 1);

	/// Request the game to generate a new random curse
	UFUNCTION(Exec)
	void RegenerateCurse();
};
