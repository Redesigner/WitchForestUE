// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"

#include "Curse.generated.h"

class UGameplayEffect;


USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FCurse
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	TArray<FGameplayTag> RequiredItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UGameplayEffect> CurseEffect;

public:
	bool CanLiftCurse(TArray<FGameplayTag> ExistingItems) const;

	void SetRequiredItems(TArray<FGameplayTag> RequiredItemsIn);

	TArray<FGameplayTag> GetRequiredItems() const { return RequiredItems; }
};
