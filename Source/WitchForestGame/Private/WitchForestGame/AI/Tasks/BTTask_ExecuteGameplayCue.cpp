// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_ExecuteGameplayCue.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

UBTTask_ExecuteGameplayCue::UBTTask_ExecuteGameplayCue()
{
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UBTTask_ExecuteGameplayCue::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComponent.GetAIOwner();
	if (!AIOwner)
	{
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* AbilitySystem = Cast<IAbilitySystemInterface>(AIOwner->GetPawn());
	if (!AbilitySystem)
	{
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = AbilitySystem->GetAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	ASC->ExecuteGameplayCue(GameplayCue);
	return EBTNodeResult::Succeeded;
}