// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/EQS/Context/EnvQueryContext_LastPerceived.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"

void UEnvQueryContext_LastPerceived::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	APawn* QueryOwner = Cast<APawn>(QueryInstance.Owner.Get());
	if (!QueryOwner)
	{
		return;
	}

	AAIController* Controller = Cast<AAIController>(QueryOwner->GetController());
	if (!Controller)
	{
		return;
	}

	if (!Controller->GetAIPerceptionComponent())
	{
		return;
	}

	TArray<AActor*> PerceivedActors;
	Controller->GetAIPerceptionComponent()->GetPerceivedHostileActors(PerceivedActors);
	if (PerceivedActors.IsEmpty())
	{
		return;
	}
	UEnvQueryItemType_Point::SetContextHelper(ContextData, PerceivedActors[0]->GetActorLocation());
}
