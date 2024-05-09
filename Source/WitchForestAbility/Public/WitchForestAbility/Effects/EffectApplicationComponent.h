// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EffectApplicationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTABILITY_API UEffectApplicationComponent : public UActorComponent
{
	GENERATED_BODY()

	
	TArray<FGameplayEffectSpecHandle> EffectHandles;

};
