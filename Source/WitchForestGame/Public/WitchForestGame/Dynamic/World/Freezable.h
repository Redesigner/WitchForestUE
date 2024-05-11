// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Freezable.generated.h"

UCLASS()
class WITCHFORESTGAME_API AFreezable : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Freezing, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float FreezeTime = 5.0f;

	FTimerHandle FreezeTimer;

	
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void Freeze();

	void Unfreeze();
};
