// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/AI/Nodes/BTComposite_Random.h"

int32 UBTComposite_Random::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	// If this node doesn't have children or it wasn't just initialized, exit
	if (GetChildrenNum() == 0 || PrevChild != BTSpecialChild::NotInitialized)
	{
		return BTSpecialChild::ReturnToParent;
	}

	return FMath::RandRange(0, GetChildrenNum() - 1);
}
