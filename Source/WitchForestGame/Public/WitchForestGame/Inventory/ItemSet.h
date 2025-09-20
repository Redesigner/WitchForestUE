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
	/// Item icon, to be displayed in the UI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInterface> ItemIcon;

	/// The pickup associated with this tag
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<APickup> PickupClass;

	/// Preview actor, for displaying in the world with something like an itemcontainer
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AActor> PreviewActor;

	/// A friendly name for the item
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
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

	bool FindItemTagFromString(FString InputString, FInventoryItemData& DataOut) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "ItemTag"))
	TArray<FItemSetEntry> Items;
};
