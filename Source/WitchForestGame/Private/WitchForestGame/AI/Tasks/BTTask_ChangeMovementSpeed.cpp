// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_ChangeMovementSpeed.h"

#include "WitchForestGame.h"

#include "GameFramework/CharacterMovementComponent.h"
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

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!MovementComponent)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ChangeMovementSpeed '{Node}' failed. Controller '{ControllerName}' does not have a valid EnemyMovementComponent.", GetName(), OwnerController->GetName());
		return EBTNodeResult::Failed;
	}

	MovementComponent->MaxWalkSpeed = MovementSpeed;
	return EBTNodeResult::Succeeded;
}
