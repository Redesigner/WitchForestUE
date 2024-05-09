// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GameplayEffectTypes.h" 

#include "AreaOfEffectActor.generated.h"

UCLASS()
class WITCHFORESTABILITY_API AAreaOfEffectActor : public AActor
{
	GENERATED_BODY()
	

	TArray<FGameplayEffectSpecHandle> EffectHandles;

};
