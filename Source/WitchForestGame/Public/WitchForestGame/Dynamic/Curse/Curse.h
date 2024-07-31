// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"

#include "Curse.generated.h"

class UGameplayEffect;

UCLASS(DisplayName = Curse, Blueprintable, BlueprintType)
class WITCHFORESTGAME_API UCurse : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	TArray<FGameplayTag> RequiredItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UGameplayEffect> CurseEffect;

public:
	bool CanLiftCurse(TArray<FGameplayTag> ExistingItems) const;
};
