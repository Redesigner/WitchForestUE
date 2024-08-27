// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_LastPerceived.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UEnvQueryContext_LastPerceived : public UEnvQueryContext
{
	GENERATED_BODY()
	
	void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
