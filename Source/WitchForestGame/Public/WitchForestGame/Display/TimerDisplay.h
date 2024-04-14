// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerDisplay.generated.h"

UCLASS()
class WITCHFORESTGAME_API ATimerDisplay : public AActor
{
	GENERATED_BODY()
	
	ATimerDisplay();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetTimerHandle(FTimerHandle TimerHandle);
};
