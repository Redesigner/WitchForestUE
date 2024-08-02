// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestUI/WitchForestActivatableWidget.h"

#include "WitchForestUI/Notifications/NotificationDataAsset.h"

#include "NotificationWidget.generated.h"

class UTextBlock;

UCLASS()
class WITCHFORESTUI_API UNotificationWidget : public UWitchForestActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UTextBlock> SubtitleText;

public:
	void SetNotification(FNotificationDataAsset_Entry Notification);
};
