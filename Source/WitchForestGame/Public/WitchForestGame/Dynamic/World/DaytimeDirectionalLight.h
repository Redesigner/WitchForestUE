// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "DaytimeDirectionalLight.generated.h"

class UDirectionalLightComponent;

UCLASS()
class WITCHFORESTGAME_API ADaytimeDirectionalLight : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDirectionalLightComponent> SunLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> LightIntensityCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> LightAngleCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> LightTemperatureCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Curves, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> RampAdjustmentCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Material, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialParameterCollection> RampedMaterialParameterCollection;


	ADaytimeDirectionalLight();
	
	void Tick(float DeltaSeconds) override;

	float GetLightIntensity(float DaytimePercentage) const;
	FQuat GetLightAngle(float DaytimePercentage) const;
	float GetLightTemperature(float DaytimePercentage) const;

	void UpdateRelevantMaterialCollectionParams(float DaytimePercentage);
};
