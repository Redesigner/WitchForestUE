// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_ChangeMovementSpeed.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

#include "Logging/StructuredLog.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_ChangeMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComponent.GetAIOwner();
	if (!OwnerController)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ChangeMovementSpeed '{Node}' failed. Controller was invalid.", GetName());
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ChangeMovementSpeed '{Node}' failed. Controller '{ControllerName}' does not have a valid pawn.", GetName(), OwnerController->GetName());
		return EBTNodeResult::Failed;
	}

	UEnemyMovementComponent* EnemyMovementComponent = Cast<UEnemyMovementComponent>(Pawn->GetMovementComponent());
	if (!EnemyMovementComponent)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ChangeMovementSpeed '{Node}' failed. Controller '{ControllerName}' does not have a valid EnemyMovementComponent.", GetName(), OwnerController->GetName());
		return EBTNodeResult::Failed;
	}

	EnemyMovementComponent->SetMaxSpeed(MovementSpeed);
	return EBTNodeResult::Succeeded;
}
