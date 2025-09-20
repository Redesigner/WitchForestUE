// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/SunMoon.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "WitchForestGame/Game/WitchForestGameState.h"

void ASunMoon::BeginPlay()
{
	Super::BeginPlay();

	Pause();
}

void ASunMoon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!RampedMaterialParameterCollection || !RampAdjustmentCurve)
	{
		return;
	}

	UMaterialParameterCollectionInstance* MaterialParameters = GetWorld()->GetParameterCollectionInstance(RampedMaterialParameterCollection);
	if (!MaterialParameters)
	{
		return;
	}

	AWitchForestGameState* GameState = Cast<AWitchForestGameState>(UGameplayStatics::GetGameState(this));
	const float DaytimePercentage = GameState ? GameState->GetDayTime() : 0.0f;
	const float DaytimePercentageScaled = 1.0f / 16.0f * FMath::Pow(4.0f * DaytimePercentage - 2.0f, 2.0f) + 0.5f;

	const float AdjustmentValue = RampAdjustmentCurve->GetFloatValue(DaytimePercentage);
	MaterialParameters->SetScalarParameterValue(TEXT("IncomingLuminanceAdjustment"), AdjustmentValue);
	MaterialParameters->SetScalarParameterValue(TEXT("OutgoingLuminanceAdjustment"), 1.0f / AdjustmentValue);
	SetTimeOfDay(DaytimePercentageScaled * 24.0f);
}
