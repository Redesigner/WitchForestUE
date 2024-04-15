// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_MovementRotation.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "WitchForestGame/Character/Enemies/Enemy.h"

EBTNodeResult::Type UBTTask_MovementRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComponent.GetAIOwner();
	if (!OwnerController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!MovementComponent)
	{
		return EBTNodeResult::Failed;
	}

	MovementComponent->bOrientRotationToMovement = bValueToSet;
	return EBTNodeResult::Succeeded;
}
