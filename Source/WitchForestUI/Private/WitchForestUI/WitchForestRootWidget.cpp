// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestRootWidget.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Logging/StructuredLog.h"

#include "WitchForestUI.h"
#include "WitchForestUI/Notifications/NotificationWidget.h"
#include "WitchForestUI/Notifications/NotificationDataAsset.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

void UWitchForestRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.RegisterListener(WitchForestGameplayTags::Event_Notification, this, &ThisClass::OnNotification);
}

void UWitchForestRootWidget::OnNotification(FGameplayTag Channel, const FWitchForestMessage& Payload)
{
	UE_LOG(LogTemp, Display, TEXT("Game notification sent"));

	if (!NotificationWidgetClass)
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "RootWidget '{Name}' received a notification with GameplayTag '{TagName}', but no notification widget was found.", GetName(), Payload.Data.ToString());
		return;
	}

	if (!NotificationData)
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "RootWidget '{Name}' received a notification with GameplayTag '{TagName}', but it does not have a valid NotificationDataAsset.", GetName(), Payload.Data.ToString());
		return;
	}

	FNotificationDataAsset_Entry NotificationEntry;
	if (!NotificationData->FindDataFromTag(Payload.Data, NotificationEntry))
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "RootWidget '{Name}' received a notification with GameplayTag '{TagName}', but there was no corresponding entry in the NotificationData.", GetName(), Payload.Data.ToString());
		return;
	}

	UNotificationWidget* NotificationWidget = CreateWidget<UNotificationWidget>(this, NotificationWidgetClass);
	NotificationWidget->SetNotification(NotificationEntry);
	NotificationWidget->AddToViewport();
	NotificationWidget->ActivateWidget();
}
