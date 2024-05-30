// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Look.generated.h"


struct FBTLook_Memory
{
	TWeakObjectPtr<APawn> OwningPawn;
	float DesiredYaw = 0.0f;
};

UCLASS()
class WITCHFORESTGAME_API UBTTask_Look : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Angle, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float Precision = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Angle, meta = (AllowPrivateAccess = true))
	float RotationAngleYaw = -20.0f;

	UBTTask_Look();
	
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	uint16 GetInstanceMemorySize() const override;
};
