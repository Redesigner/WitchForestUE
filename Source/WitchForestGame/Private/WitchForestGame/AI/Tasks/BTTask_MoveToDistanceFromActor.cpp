// Copyright (c) 2024 Stephen Melnick, Epic Games


#include "WitchForestGame/AI/Tasks/BTTask_MoveToDistanceFromActor.h"

#include "NavigationSystem.h"
#include "Logging/StructuredLog.h"
#include "AISystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Tasks/AITask_MoveTo.h"

#include "WitchForestGame.h"

using MemoryType = FBTMoveToDistanceFromActorMemory;


// This class is *mostly* just a rehash of BTTask_MoveTo, with my own comments added
UBTTask_MoveToDistanceFromActor::UBTTask_MoveToDistanceFromActor()
{
	NodeName = "Move to Distance from Actor";
	INIT_TASK_NODE_NOTIFY_FLAGS();

	AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);
	bReachTestIncludesGoalRadius = bReachTestIncludesAgentRadius = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
	bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);
	bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);
	bTrackMovingGoal = true;
	bRequireNavigableEndLocation = true;
	bProjectGoalLocation = true;

	ObservedBlackboardValueTolerance = AcceptableRadius * 0.95f;

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToDistanceFromActor, BlackboardKey), AActor::StaticClass());
}

// Set up our Move Task's parameters, and then execute them
EBTNodeResult::Type UBTTask_MoveToDistanceFromActor::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(NodeMemory);
	AAIController* MyController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	if (!MyController || !MyBlackboard)
	{
		return NodeResult;
	}
	UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	AActor* TargetActor = Cast<AActor>(KeyValue);
	AActor* OwnerAvatar = MyController->GetPawn();
	if (!TargetActor || !OwnerAvatar)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "UBTTask_MoveToDistanceFromActor: \'{NodeName}\' MoveTask failed. Pawn was nullptr.", GetNodeName());
		return NodeResult;
	}
	FAIMoveRequest MoveReq;
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : MyController->GetDefaultNavigationFilterClass());
	MoveReq.SetAllowPartialPath(bAllowPartialPath);
	MoveReq.SetCanStrafe(bAllowStrafe);
	MoveReq.SetReachTestIncludesAgentRadius(bReachTestIncludesAgentRadius);
	MoveReq.SetReachTestIncludesGoalRadius(bReachTestIncludesGoalRadius);
	MoveReq.SetRequireNavigableEndLocation(bRequireNavigableEndLocation);
	MoveReq.SetUsePathfinding(false);
	MoveReq.SetGoalLocation(CalculateDesiredLocation(OwnerAvatar, TargetActor));

	if (!MoveReq.IsValid())
	{
		return NodeResult;
	}
	UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
	const bool bReuseExistingTask = (MoveTask != nullptr);

	MoveTask = PrepareMoveTask(OwnerComp, MoveTask, MoveReq);
	// Failed to generate the move task, for some reason...
	if (!MoveTask)
	{
		return NodeResult;
	}
	// I'm not really sure why this variable is set to false and then true immediately after...
	MyMemory->bObserverCanFinishTask = false;

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
	MyMemory->bObserverCanFinishTask = true;
	NodeResult = (MoveTask->GetState() != EGameplayTaskState::Finished) ? EBTNodeResult::InProgress :
		MoveTask->WasMoveSuccessful() ? EBTNodeResult::Succeeded :
		EBTNodeResult::Failed;
	return NodeResult;
}


UAITask_MoveTo* UBTTask_MoveToDistanceFromActor::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest)
{
	UAITask_MoveTo* MoveTask = ExistingTask ? ExistingTask : NewBTAITask<UAITask_MoveTo>(OwnerComp);
	if (MoveTask)
	{
		MoveTask->SetUp(MoveTask->GetAIController(), MoveRequest);
	}
	return MoveTask;
}

FVector UBTTask_MoveToDistanceFromActor::CalculateDesiredLocation(AActor* Avatar, AActor* Target) const
{
	FVector DeltaLocation = Target->GetActorLocation() - Avatar->GetActorLocation();
	FVector Direction = DeltaLocation.GetSafeNormal2D();
	return Target->GetActorLocation() - Direction * Distance;
}


EBTNodeResult::Type UBTTask_MoveToDistanceFromActor::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	FBTMoveToDistanceFromActorMemory* MyMemory = CastInstanceNodeMemory<FBTMoveToDistanceFromActorMemory>(NodeMemory);
	MyMemory->PreviousGoalLocation = FAISystem::InvalidLocation;
	MyMemory->MoveRequestID = FAIRequestID::InvalidRequest;

	AAIController* MyController = OwnerComponent.GetAIOwner();
	if (MyController == nullptr)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "UBTTask_MoveToDistanceFromActor::ExecuteTask failed since AIController is missing.");
		NodeResult = EBTNodeResult::Failed;
	}
	else
	{
		NodeResult = PerformMoveTask(OwnerComponent, NodeMemory);
	}

	if (NodeResult != EBTNodeResult::InProgress)
	{
		return NodeResult;
	}
	UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
	if (!ensure(BlackboardComp))
	{
		return EBTNodeResult::Failed;
	}
	if (MyMemory->BBObserverDelegateHandle.IsValid())
	{
		BlackboardComp->UnregisterObserver(BlackboardKey.GetSelectedKeyID(), MyMemory->BBObserverDelegateHandle);
	}
	MyMemory->BBObserverDelegateHandle = BlackboardComp->RegisterObserver(BlackboardKey.GetSelectedKeyID(), this, FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnBlackboardValueChange));
	return NodeResult;
}


EBTNodeResult::Type UBTTask_MoveToDistanceFromActor::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(NodeMemory);
	if (MyMemory->MoveRequestID.IsValid())
	{
		AAIController* MyController = OwnerComp.GetAIOwner();
		if (MyController && MyController->GetPathFollowingComponent())
		{
			//MyController->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished, MyMemory->MoveRequestID);
		}
	}
	else
	{
		MyMemory->bObserverCanFinishTask = false;
		UAITask_MoveTo* MoveTask = MyMemory->Task.Get();
		if (MoveTask)
		{
			MoveTask->ExternalCancel();
		}
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}


void UBTTask_MoveToDistanceFromActor::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	FBTMoveToDistanceFromActorMemory* MyMemory = CastInstanceNodeMemory<FBTMoveToDistanceFromActorMemory>(NodeMemory);
	MyMemory->Task.Reset();

	if (bObserveBlackboardValue)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (ensure(BlackboardComp) && MyMemory->BBObserverDelegateHandle.IsValid())
		{
			BlackboardComp->UnregisterObserver(BlackboardKey.GetSelectedKeyID(), MyMemory->BBObserverDelegateHandle);
		}
		MyMemory->BBObserverDelegateHandle.Reset();
	}
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}


uint16 UBTTask_MoveToDistanceFromActor::GetInstanceMemorySize() const
{
	return sizeof(FBTMoveToDistanceFromActorMemory);
}


void UBTTask_MoveToDistanceFromActor::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UAITask_MoveTo* MoveTask = Cast<UAITask_MoveTo>(&Task);
	if (MoveTask && MoveTask->GetAIController() && MoveTask->GetState() != EGameplayTaskState::Paused)
	{
		UBehaviorTreeComponent* BehaviorComp = GetBTComponentForTask(Task);
		if (BehaviorComp)
		{
			uint8* RawMemory = BehaviorComp->GetNodeMemory(this, BehaviorComp->FindInstanceContainingNode(this));
			const MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(RawMemory);

			if (MyMemory && MyMemory->bObserverCanFinishTask && (MoveTask == MyMemory->Task))
			{
				const bool bSuccess = MoveTask->WasMoveSuccessful();
				FinishLatentTask(*BehaviorComp, bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
			}
		}
	}
}


void UBTTask_MoveToDistanceFromActor::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
	bSuccess &= (Message != UBrainComponent::AIMessage_RepathFailed);
	Super::OnMessage(OwnerComp, NodeMemory, Message, RequestID, bSuccess);
}


EBlackboardNotificationResult UBTTask_MoveToDistanceFromActor::OnBlackboardValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	const AAIController* MyController = BehaviorComp->GetAIOwner();
	uint8* RawMemory = BehaviorComp->GetNodeMemory(this, BehaviorComp->FindInstanceContainingNode(this));
	MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(RawMemory);

	const EBTTaskStatus::Type TaskStatus = BehaviorComp->GetTaskStatus(this);
	if (TaskStatus != EBTTaskStatus::Active)
	{
		MyMemory->BBObserverDelegateHandle.Reset();
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (MyMemory == nullptr || BehaviorComp->GetAIOwner() == nullptr)
	{
		return EBlackboardNotificationResult::ContinueObserving;
	}
	check(BehaviorComp->GetAIOwner()->GetPathFollowingComponent());

	// don't abort move if using AI tasks - it will mess things up
	if (MyMemory->MoveRequestID.IsValid())
	{
		StopWaitingForMessages(*BehaviorComp);
		// BehaviorComp->GetAIOwner()->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::NewRequest, MyMemory->MoveRequestID, EPathFollowingVelocityMode::Keep);
	}

	const EBTNodeResult::Type NodeResult = PerformMoveTask(*BehaviorComp, RawMemory);
	if (NodeResult != EBTNodeResult::InProgress)
	{
		FinishLatentTask(*BehaviorComp, NodeResult);
	}
	return EBlackboardNotificationResult::ContinueObserving;
}


void UBTTask_MoveToDistanceFromActor::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}
	const FString KeyValue = BlackboardComp->DescribeKeyValue(BlackboardKey.GetSelectedKeyID(), EBlackboardDescription::OnlyValue);
	const MemoryType* MyMemory = CastInstanceNodeMemory<MemoryType>(NodeMemory);
	const bool bIsUsingTask = MyMemory->Task.IsValid();
	const FString ModeDesc = bIsUsingTask ? TEXT("(task)") : TEXT("");

	Values.Add(FString::Printf(TEXT("move target: %s%s"), *KeyValue, *ModeDesc));
}


FString UBTTask_MoveToDistanceFromActor::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}
