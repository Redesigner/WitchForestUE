// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_Wander.h"

#include "NavigationSystem.h"
#include "Logging/StructuredLog.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Tasks/AITask_MoveTo.h"

#include "WitchForestGame.h"

using MemoryType = FBTWanderMemory;


UBTTask_Wander::UBTTask_Wander()
{
}


EBTNodeResult::Type UBTTask_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	return PerformMoveTask(OwnerComponent, NodeMemory);
}


EBTNodeResult::Type UBTTask_Wander::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(NodeMemory);
	if (MyMemory->MoveRequestID.IsValid())
	{
		AAIController* MyController = OwnerComp.GetAIOwner();
		if (MyController && MyController->GetPathFollowingComponent())
		{
			MyController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, MyMemory->MoveRequestID);
		}
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}


void UBTTask_Wander::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(NodeMemory);
	MyMemory->Task.Reset();

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}


uint16 UBTTask_Wander::GetInstanceMemorySize() const
{
	return sizeof(FBTWanderMemory);
}

void UBTTask_Wander::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
	bSuccess &= (Message != UBrainComponent::AIMessage_RepathFailed);
	Super::OnMessage(OwnerComp, NodeMemory, Message, RequestID, bSuccess);
}


EBTNodeResult::Type UBTTask_Wander::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(NodeMemory);
	AAIController* MyController = OwnerComp.GetAIOwner();

	if (!MyController || !MyBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	AActor* OwnerAvatar = MyController->GetPawn();
	if (!OwnerAvatar)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "UBTTask_Wander: \'{NodeName}\' MoveTask failed. Pawn was nullptr.", GetNodeName());
		return EBTNodeResult::Failed;
	}

	FAIMoveRequest MoveReq;
	MoveReq.SetAllowPartialPath(true);
	MoveReq.SetCanStrafe(true);
	MoveReq.SetUsePathfinding(false);
	MoveReq.SetGoalLocation(GetDesiredLocation(OwnerAvatar, WanderRadius));

	if (!MoveReq.IsValid())
	{
		return EBTNodeResult::Failed;
	}

	UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
	const bool bReuseExistingTask = (MoveTask != nullptr);

	MoveTask = PrepareMoveTask(OwnerComp, MoveTask, MoveReq);
	if (!MoveTask)
	{
		return EBTNodeResult::Failed;
	}

	// I'm not really sure why this variable is set to false and then true immediately after...
	if (bReuseExistingTask)
	{
		if (MoveTask->IsActive())
		{
			MoveTask->ConditionalPerformMove();
		}
	}
	else
	{
		MyMemory->Task = MoveTask;
		MoveTask->ReadyForActivation();
	}

	EGameplayTaskState GameplayTaskState = MoveTask->GetState();
	if (GameplayTaskState != EGameplayTaskState::Finished)
	{
		return EBTNodeResult::InProgress;
	}

	if (MoveTask->WasMoveSuccessful())
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}


UAITask_MoveTo* UBTTask_Wander::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest)
{
	UAITask_MoveTo* MoveTask = ExistingTask ? ExistingTask : NewBTAITask<UAITask_MoveTo>(OwnerComp);
	if (MoveTask)
	{
		MoveTask->SetUp(MoveTask->GetAIController(), MoveRequest);
	}
	return MoveTask;
}

const FVector UBTTask_Wander::GetDesiredLocation(AActor* OwnerActor, float Radius)
{
	UNavigationSystemBase* NavigationSystemBase = GetWorld()->GetNavigationSystem();
	UNavigationSystemV1* NavigationSystem = Cast<UNavigationSystemV1>(NavigationSystemBase);
	if (!NavigationSystem)
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "Unable to execute BehaviorTree task '{TaskName}'. The task requires NavigationSystemV1.", GetName());
		return FVector();
	}

	FNavLocation WanderLocation;
	const bool bSuccess = NavigationSystem->GetRandomReachablePointInRadius(OwnerActor->GetActorLocation(), WanderRadius, WanderLocation);
	if (!bSuccess)
	{
		return FVector();
	}

	return WanderLocation;
}
