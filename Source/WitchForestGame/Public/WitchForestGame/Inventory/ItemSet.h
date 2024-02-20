 // Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "ItemSet.generated.h"

WITCHFORESTGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_ItemEmpty);

class APickup;

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FInventoryItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> ItemIcon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickup> PickupClass;

	UPROPERTY(EditDefaultsOnly)
	FName ItemName;
};

USTRUCT(BlueprintType)
struct FItemSetEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FInventoryItemData ItemData;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "ItemTag"))
	FGameplayTag ItemTag;
};


UCLASS()
class WITCHFORESTGAME_API UItemSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	bool FindItemDataForTag(const FGameplayTag& InputTag, FInventoryItemData& DataOut) const;

	bool FindItemTagFromClass(TSubclassOf<APickup> InputClass, FGameplayTag& TagOut) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "ItemTag"))
	TArray<FItemSetEntry> Items;
};
