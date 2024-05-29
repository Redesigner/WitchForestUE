// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"

#include "BTTask_ExecuteAbilityTag.generated.h"

struct FAbilityEndedData;

UCLASS()
class WITCHFORESTGAME_API UBTTask_ExecuteAbilityTag : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (AllowPrivateAccess = true))
	FGameplayTagContainer AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (AllowPrivateAccess = true))
	bool bAllowAbilityReactivation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (AllowPrivateAccess = true))
	bool bAbortAbility = false;


	TWeakObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	FGameplayAbilitySpecHandle ActivatedAbility;


	UBTTask_ExecuteAbilityTag();

	void OnInstanceCreated(UBehaviorTreeComponent& OwnerComp) override;

	EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
