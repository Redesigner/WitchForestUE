// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"
// #include "Widgets/CommonActivatableWidgetContainer.h"

#include "WitchForestLayoutMap.generated.h"

class UCommonActivatableWidget;

USTRUCT(BlueprintType)
struct WITCHFORESTUI_API FWitchForestLayoutMapEntry
{
	GENERATED_BODY()

	// The tag received with the message
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "UI.MessageTag"))
	FGameplayTag MessageTag;

	// The slot to put this widget into
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "UI.SlotTag"))
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> Widget;
};

UCLASS()
class WITCHFORESTUI_API UWitchForestLayoutMap : public UPrimaryDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FWitchForestLayoutMapEntry> LayoutEntries;

public:	
	bool FindWidgetLayoutForMessage(FGameplayTag MessageTag, FWitchForestLayoutMapEntry& WidgetLayoutOut) const;
};
