// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "DropTable.generated.h"


USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FDropTableAmount
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "ItemTag"))
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly)
	uint8 Amount = 1;
};

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FDropTableEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float DropRate = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	FDropTableAmount ItemAmount;
};

UCLASS()
class WITCHFORESTGAME_API UDropTable : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "ItemAmount"))
	TArray<FDropTableEntry> Entries;

	TArray<FDropTableAmount> Evaluate() const;
};
