// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_Look.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"


UBTTask_Look::UBTTask_Look()
{
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UBTTask_Look::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComponent.GetAIOwner();
	if (!AIOwner)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIOwner->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	FBTLook_Memory* LookMemory = CastInstanceNodeMemory<FBTLook_Memory>(NodeMemory);

	FRotator DesiredRotation = Pawn->GetActorRotation();
	DesiredRotation.Yaw += RotationAngleYaw;
	AIOwner->SetControlRotation(DesiredRotation);

	float DesiredYaw = DesiredRotation.Yaw;
	if (DesiredYaw > 180.0f)
	{
		DesiredYaw -= 360.0f;
	}
	else if (DesiredYaw < -180.0f)
	{
		DesiredYaw += 360.0f;
	}
	LookMemory->DesiredYaw = DesiredYaw;
	LookMemory->OwningPawn = Pawn;

	return EBTNodeResult::InProgress;
}

void UBTTask_Look::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FBTLook_Memory* LookMemory = CastInstanceNodeMemory<FBTLook_Memory>(NodeMemory);
	if (LookMemory->OwningPawn.IsValid())
	{
		const float CurrentYaw = LookMemory->OwningPawn->GetActorRotation().Yaw;
		const float DesiredYaw = LookMemory->DesiredYaw;
		float DifferenceYaw = DesiredYaw - CurrentYaw;
		if (DifferenceYaw > 180.0f)
		{
			DifferenceYaw -= 360.0f;
		}
		// UE_LOG(LogTemp, Display, TEXT("Desired yaw: %f, Current yaw: %f, Difference yaw: %f"), DesiredYaw, CurrentYaw, DifferenceYaw);

		if (FMath::Abs(DifferenceYaw) <= Precision)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

uint16 UBTTask_Look::GetInstanceMemorySize() const
{
	return sizeof(FBTLook_Memory);
}
