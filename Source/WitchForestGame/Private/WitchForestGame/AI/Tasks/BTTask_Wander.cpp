// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_Wander.h"

#include "NavigationSystem.h"
#include "Logging/StructuredLog.h"
#include "AIController.h"

#include "WitchForestGame.h"

UBTTask_Wander::UBTTask_Wander()
{
	bCreateNodeInstance = true;
}

void UBTTask_Wander::OnInstanceCreated(UBehaviorTreeComponent& OwnerComp)
{
	Super::OnInstanceCreated(OwnerComp);
	BehaviorTreeComponent = &OwnerComp;
}

EBTNodeResult::Type UBTTask_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComponent.GetAIOwner();
	if (!OwnerController)
	{
		return EBTNodeResult::Failed;
	}
	UNavigationSystemBase* NavigationSystemBase = GetWorld()->GetNavigationSystem();
	UNavigationSystemV1* NavigationSystem = Cast<UNavigationSystemV1>(NavigationSystemBase);
	if (!NavigationSystem)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to execute BehaviorTree task '{TaskName}'. The task requires NavigationSystemV1.", GetName());
		return EBTNodeResult::Failed;
	}
	APawn* EnemyPawn = OwnerController->GetPawn();
	if (!EnemyPawn)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to execute BehaviorTree task '{TaskName}'. The Pawn belonging to '{ControllerName}' is invalid.", GetName(), OwnerController->GetName());
		return EBTNodeResult::Failed;
	}

	FNavLocation WanderLocation;
	const bool bSuccess = NavigationSystem->GetRandomReachablePointInRadius(EnemyPawn->GetActorLocation(), WanderRadius, WanderLocation);
	if (!bSuccess)
	{
		return EBTNodeResult::Failed;
	}
	EPathFollowingRequestResult::Type PathResult = OwnerController->MoveToLocation(WanderLocation, -1.0f, true, true, false, false);
	if (PathResult == EPathFollowingRequestResult::Type::Failed)
	{
		return EBTNodeResult::Failed;
	}
	if (PathResult == EPathFollowingRequestResult::Type::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}
	OwnerController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UBTTask_Wander::PathRequestCompleted);
	return EBTNodeResult::InProgress;
}

void UBTTask_Wander::PathRequestCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
}
