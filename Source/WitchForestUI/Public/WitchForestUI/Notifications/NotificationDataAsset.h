// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "NotificationDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FNotificationDataAsset_Entry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Notification"))
	FGameplayTag NotificationTag;

	UPROPERTY(EditDefaultsOnly)
	FText NotificationTitle;

	UPROPERTY(EditDefaultsOnly)
	FText NotificationSubtitle;
};

UCLASS(BlueprintType, Const)
class WITCHFORESTUI_API UNotificationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	bool FindDataFromTag(FGameplayTag Tag, FNotificationDataAsset_Entry& EntryOut);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Notification data", meta = (TitleProperty = NotificationTitle))
	TArray<FNotificationDataAsset_Entry> NotificationSet;

};