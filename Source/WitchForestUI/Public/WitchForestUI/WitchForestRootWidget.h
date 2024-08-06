// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestUI/WitchForestActivatableWidget.h"

#include "WitchForestGame/Messages/WitchForestMessage.h"

#include "WitchForestRootWidget.generated.h"

class UNotificationDataAsset;
class UNotificationWidget;

UCLASS()
class WITCHFORESTUI_API UWitchForestRootWidget : public UWitchForestActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Notifications, meta = (AllowPrivateAccess = true))
	TObjectPtr<UNotificationDataAsset> NotificationData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Notifications, meta = (AllowPrivateAccess = true))
	TSubclassOf<UNotificationWidget> NotificationWidgetClass;

	void NativeConstruct() override;

	void OnNotification(FGameplayTag Channel, const FWitchForestMessage& Payload);
};
