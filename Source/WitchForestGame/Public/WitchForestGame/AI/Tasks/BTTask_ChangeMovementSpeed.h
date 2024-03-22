// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UBTTask_ChangeMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float MovementSpeed = 500.0f;


	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
};
