// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Tasks/BTTask_ExecuteAbilityTag.h"

#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"

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
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ExecuteAbilityTag '{Node}' failed. Controller was invalid.", GetName());
		return EBTNodeResult::Failed;
	}
	APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ExecuteAbilityTag '{Node}' failed. Controller '{ControllerName}' does not have a valid pawn.", GetName(), OwnerController->GetName());
		return EBTNodeResult::Failed;
	}
	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!AbilityInterface)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ExecuteAbilityTag '{Node}' failed. Pawn '{PawnName}' does not implement the IAbilitySystemInterface.", GetName(), Pawn->GetName());
		return EBTNodeResult::Failed;
	}
	UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent();
	ASC->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
	TArray<FGameplayAbilitySpecHandle> FoundSpecHandles;
	ASC->FindAllAbilitiesWithTags(FoundSpecHandles, AbilityTag);
	if (FoundSpecHandles.Num() <= 0)
	{
		UE_LOGFMT(LogWitchForestAI, Error, "UBTTask_ExecuteAbilityTag '{Node}' failed. Could not find an ability matching AbilityTag.", GetName());
		return EBTNodeResult::Failed;
	}
	ActivatedAbility = FoundSpecHandles[0];
	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(ActivatedAbility);
	// Don't even try to reactivate the ability if it's active, just in case
	if (!bAllowAbilityReactivation && AbilitySpec && AbilitySpec->IsActive())
	{
		UE_LOGFMT(LogWitchForestAI, Display, "UBTTask_ExecuteAbilityTag '{Node}' cancelled by self. Ability '{AbilityName}' is already active, and `bAllowAbilityReactivation` is set to false.", GetName(), AbilitySpec->Ability->GetName());
		return EBTNodeResult::Failed;
	}
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
