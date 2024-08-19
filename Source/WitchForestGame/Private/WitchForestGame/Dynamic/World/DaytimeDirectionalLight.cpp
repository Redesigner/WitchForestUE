// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/DaytimeDirectionalLight.h"

#include "Components/DirectionalLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "WitchForestGame/Game/WitchForestGameState.h"


ADaytimeDirectionalLight::ADaytimeDirectionalLight()
{
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	// PrimaryActorTick.TickInterval = 0.25f;
}

void ADaytimeDirectionalLight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AWitchForestGameState* GameState = Cast<AWitchForestGameState>(UGameplayStatics::GetGameState(this));
	float DaytimePercentage = GameState ? GameState->GetDayTime() : 0.0f;

	SetActorRotation(GetLightAngle(DaytimePercentage));
	SunLight->SetIntensity(GetLightIntensity(DaytimePercentage));
	SunLight->SetTemperature(GetLightTemperature(DaytimePercentage));
	UpdateRelevantMaterialCollectionParams(DaytimePercentage);
}

float ADaytimeDirectionalLight::GetLightIntensity(float DaytimePercentage) const
{
	if (!LightIntensityCurve)
	{
		return 5.0f;
	}

	return LightIntensityCurve->GetFloatValue(DaytimePercentage);
}

FQuat ADaytimeDirectionalLight::GetLightAngle(float DaytimePercentage) const
{
	if (!LightAngleCurve)
	{
		return FQuat::Slerp(
			FQuat::MakeFromEuler(FVector(60.0f, 0.0f, 0.0f)),
			FQuat::MakeFromEuler(FVector(60.0f, -180.0f, 0.0f)),
			DaytimePercentage
		);
	}

	return FQuat::MakeFromEuler(FVector(60.0f, LightAngleCurve->GetFloatValue(DaytimePercentage), 0.0f));
}

float ADaytimeDirectionalLight::GetLightTemperature(float DaytimePercentage) const
{
	if (!LightTemperatureCurve)
	{
		return 5000.0f;
	}
	return LightTemperatureCurve->GetFloatValue(DaytimePercentage);
}

void ADaytimeDirectionalLight::UpdateRelevantMaterialCollectionParams(float DaytimePercentage)
{
	if (!RampedMaterialParameterCollection || !RampAdjustmentCurve)
	{
		return;
	}

	UMaterialParameterCollectionInstance* MaterialParameters = GetWorld()->GetParameterCollectionInstance(RampedMaterialParameterCollection);
	if (!MaterialParameters)
	{
		return;
	}

	float AdjustmentValue = RampAdjustmentCurve->GetFloatValue(DaytimePercentage);
	MaterialParameters->SetScalarParameterValue(TEXT("IncomingLuminanceAdjustment"), AdjustmentValue);
	MaterialParameters->SetScalarParameterValue(TEXT("OutgoingLuminanceAdjustment"), 1.0f / AdjustmentValue);
}
