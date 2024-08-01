// Copyright (c) 2024 Stephen Melnick

#include "WitchForestUI/Notifications/NotificationDataAsset.h"

bool UNotificationDataAsset::FindDataFromTag(FGameplayTag Tag, FNotificationDataAsset_Entry& EntryOut)
{
    for (FNotificationDataAsset_Entry& DataEntry : NotificationSet)
    {
        if (DataEntry.NotificationTag == Tag)
        {
            EntryOut = DataEntry;
            return true;
        }
    }
    return false;
}
