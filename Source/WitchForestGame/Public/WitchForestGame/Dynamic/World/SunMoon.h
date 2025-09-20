// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Actors/SunMoonDaySequenceActor.h"
#include "SunMoon.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API ASunMoon : public ASunMoonDaySequenceActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> RampAdjustmentCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Material, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialParameterCollection> RampedMaterialParameterCollection;

	void BeginPlay() override;
	
	void Tick(float DeltaSeconds) override;
	
};
