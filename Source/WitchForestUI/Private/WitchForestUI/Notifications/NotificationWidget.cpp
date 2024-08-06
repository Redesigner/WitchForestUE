// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/Notifications/NotificationWidget.h"

#include "Components/TextBlock.h"

void UNotificationWidget::SetNotification(FNotificationDataAsset_Entry Notification)
{
	if (TitleText)
	{
		TitleText->SetText(Notification.NotificationTitle);
	}

	if (SubtitleText)
	{
		SubtitleText->SetText(Notification.NotificationSubtitle);
	}
}

FReply UNotificationWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	DeactivateWidget();

	return FReply::Handled();
}

void UNotificationWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	// UE_LOG(LogTemp, Error, TEXT("Notification widget lost focus! Reason: %d"), InFocusEvent.GetCause());
}

