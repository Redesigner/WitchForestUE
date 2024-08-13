// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/DaytimeDirectionalLight.h"

#include "Kismet/GameplayStatics.h"
#include "Components/DirectionalLightComponent.h"

#include "WitchForestGame/Game/WitchForestGameState.h"


ADaytimeDirectionalLight::ADaytimeDirectionalLight()
{
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	// PrimaryActorTick.TickInterval = 1.0f;
}

void ADaytimeDirectionalLight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetActorRotation(GetLightAngle());
}

FQuat ADaytimeDirectionalLight::GetLightAngle() const
{
	AWitchForestGameState* GameState = Cast<AWitchForestGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return FQuat::MakeFromEuler(FVector(00.0f, -90.0f, 0.0f));
	}

	return FQuat::Slerp(
		FQuat::MakeFromEuler(FVector(60.0f, 0.0f, 0.0f)),
		FQuat::MakeFromEuler(FVector(60.0f, -180.0f, 0.0f)),
		GameState->GetDayTime()
	);
}
