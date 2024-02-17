// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_MovementRotation.h"

#include "AIController.h"

#include "WitchForestGame/Character/Enemies/Enemy.h"
#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"

EBTNodeResult::Type UBTTask_MovementRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComponent.GetAIOwner();
	if (!OwnerController)
	{
		return EBTNodeResult::Failed;
	}
	AEnemy* Enemy = Cast<AEnemy>(OwnerController->GetPawn());
	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}
	UEnemyMovementComponent* MovementComponent = Enemy->GetEnemyMovementComponent();
	if (!MovementComponent)
	{
		return EBTNodeResult::Failed;
	}
	MovementComponent->SetOrientRotationToMovement(bValueToSet);
	return EBTNodeResult::Succeeded;
}
