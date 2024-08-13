// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DaytimeActor.generated.h"

UCLASS()
class WITCHFORESTGAME_API ADaytimeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADaytimeActor();

private:
	void BeginPlay() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDayEnd();

	virtual void DayEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDayStart();

	virtual void DayStart();

};
