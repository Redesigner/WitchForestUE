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

	ADaytimeDirectionalLight();
	
	void Tick(float DeltaSeconds) override;

	FQuat GetLightAngle() const;
};
