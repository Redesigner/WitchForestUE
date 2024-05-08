// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "BestiaryData.generated.h"

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FBestiaryEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (Categories = "CreatureTag"))
	FGameplayTag CreatureTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText CreatureName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText CreatureDescription;
};

UCLASS()
class WITCHFORESTGAME_API UBestiaryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "CreatureName"))
	TArray<FBestiaryEntry> Entries;

	bool FindBestiaryForTag(const FGameplayTag& InputTag, FBestiaryEntry& DataOut) const;

};
