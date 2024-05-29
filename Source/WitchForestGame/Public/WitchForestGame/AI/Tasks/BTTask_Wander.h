// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Navigation/PathFollowingComponent.h" 

#include "BTTask_Wander.generated.h"


class UAITask_MoveTo;

struct FBTWanderMemory
{
	/** Move request ID */
	FAIRequestID MoveRequestID;

	FDelegateHandle BBObserverDelegateHandle;

	TWeakObjectPtr<UAITask_MoveTo> Task;
};

UCLASS()
class WITCHFORESTGAME_API UBTTask_Wander : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float WanderRadius = 500.0f;
	
	UBTTask_Wander();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	uint16 GetInstanceMemorySize() const override;

	void OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess) override;

	EBTNodeResult::Type PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	UAITask_MoveTo* PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveRequest);

	const FVector GetDesiredLocation(AActor* OwnerActor, float Radius);

	void SetWandering(UBehaviorTreeComponent& OwnerComp, bool bWandering);
};
