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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AbilityTag, meta = (AllowPrivateAccess = true))
	FGameplayTagContainer AbilityTag;

	TWeakObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	FGameplayAbilitySpecHandle ActivatedAbility;



	void OnInstanceCreated(UBehaviorTreeComponent& OwnerComp) override;

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

public:
	UBTTask_ExecuteAbilityTag();

	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
