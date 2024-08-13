// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/DaytimeActor.h"

#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Game/WitchForestGameState.h"


ADaytimeActor::ADaytimeActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ADaytimeActor::BeginPlay()
{
	Super::BeginPlay();
	
	AWitchForestGameState* GameState = Cast<AWitchForestGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "DaytimeActor '{Name}' failed to bind to delegates: the game state is not of type AWitchForestGameState.", GetName());
		return;
	}

	GameState->OnDayEnd.AddUObject(this, &ThisClass::DayEnd);
	GameState->OnDayStart.AddUObject(this, &ThisClass::DayStart);
}

void ADaytimeActor::DayEnd()
{
	OnDayEnd();
}

void ADaytimeActor::DayStart()
{
	OnDayStart();
}

