// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Flammable.generated.h"

UCLASS()
class WITCHFORESTGAME_API AFlammable : public AActor
{
	GENERATED_BODY()
	
	bool bBurning = false;


	AFlammable();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void StartBurning();

	UFUNCTION(BlueprintCallable)
	void FinishBurning();
};
