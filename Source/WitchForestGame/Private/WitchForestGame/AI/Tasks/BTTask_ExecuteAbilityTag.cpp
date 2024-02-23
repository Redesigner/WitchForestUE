// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_ExecuteAbilityTag.h"

#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

void UBTTask_ExecuteAbilityTag::OnInstanceCreated(UBehaviorTreeComponent& OwnerComp)
{
	Super::OnInstanceCreated(OwnerComp);
	BehaviorTreeComponent = &OwnerComp;
}

EBTNodeResult::Type UBTTask_ExecuteAbilityTag::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
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
	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!AbilityInterface)
	{
		return EBTNodeResult::Failed;
	}
	UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent();
	ASC->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
	TArray<FGameplayAbilitySpecHandle> FoundSpecHandles;
	ASC->FindAllAbilitiesWithTags(FoundSpecHandles, AbilityTag);
	if (FoundSpecHandles.Num() <= 0)
	{
		return EBTNodeResult::Failed;
	}
	ActivatedAbility = FoundSpecHandles[0];
	if (ASC->TryActivateAbility(ActivatedAbility))
	{
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

UBTTask_ExecuteAbilityTag::UBTTask_ExecuteAbilityTag()
{
	bCreateNodeInstance = true;
}

void UBTTask_ExecuteAbilityTag::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle == ActivatedAbility && BehaviorTreeComponent.IsValid())
	{
		FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
	}
}
