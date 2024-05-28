// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Navigation/PathFollowingComponent.h" 

#include "BTTask_Wander.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UBTTask_Wander : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	float WanderRadius = 500.0f;

	TWeakObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
public:
	UBTTask_Wander();

	void OnInstanceCreated(UBehaviorTreeComponent& OwnerComp) override;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UFUNCTION()
	void PathRequestCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
