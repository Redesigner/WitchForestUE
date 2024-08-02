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
