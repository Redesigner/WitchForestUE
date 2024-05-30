// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "GameplayTagContainer.h"

#include "BTTask_ExecuteGameplayCue.generated.h"

UCLASS()
class WITCHFORESTGAME_API UBTTask_ExecuteGameplayCue : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Cue, Meta = (AllowPrivateAccess = true, Categories = "GameplayCue"))
	FGameplayTag GameplayCue;


	UBTTask_ExecuteGameplayCue();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
};
