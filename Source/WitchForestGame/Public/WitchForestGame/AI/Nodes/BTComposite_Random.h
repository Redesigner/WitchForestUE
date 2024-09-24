// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"

#include "BehaviorTree/BehaviorTreeTypes.h"

#include "BTComposite_Random.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UBTComposite_Random : public UBTCompositeNode
{
	GENERATED_BODY()
	
	int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

	void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
};
