// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/Layout/WitchForestLayout.h"

#include "CommonActivatableWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Logging/StructuredLog.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Dynamic/Interactable/ActorParameterWidgetInterface.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"

#include "WitchForestUI.h"
#include "WitchForestUI/Layout/WitchForestLayoutMap.h"
#include "WitchForestUI/Notifications/NotificationWidget.h"
#include "WitchForestUI/Notifications/NotificationDataAsset.h"


UE_DEFINE_GAMEPLAY_TAG(UI_SlotTag_Notification, "UI.SlotTag.Notification");

void UWitchForestLayout::RegisterLayer(UCommonActivatableWidgetContainerBase* Layer, FGameplayTag SlotTag)
{
	if (LayerMap.Contains(SlotTag))
	{
		return;
	}

	LayerMap.Add(SlotTag, Layer);
}

bool UWitchForestLayout::Initialize()
{
	if (!IsDesignTime())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.RegisterListener(WitchForestGameplayTags::MessageChannel_UI, this, &ThisClass::ReceiveMessage);
		MessageSubsystem.RegisterListener(WitchForestGameplayTags::MessageChannel_Notification, this, &ThisClass::ReceiveNotification);
	}

	return Super::Initialize();
}

void UWitchForestLayout::ReceiveMessage(FGameplayTag Channel, const FWitchForestUIMessage& Payload)
{
	if (!LayoutMap)
	{
		return;
	}

	FWitchForestLayoutMapEntry LayoutMapEntry;
	if (!LayoutMap->FindWidgetLayoutForMessage(Payload.MessageTag, LayoutMapEntry))
	{
		return;
	}

	UCommonActivatableWidget* NewWidget = PushWidgetToLayer(LayoutMapEntry.Widget, LayoutMapEntry.SlotTag);
	if (!NewWidget)
	{
		UE_LOGFMT(LogWitchForestUI, Error, "WitchForestLayout '{LayoutName}' failed to create new widget of type '{WidgetClassName}' for message '{WidgetMessage}'", GetName(), LayoutMapEntry.Widget.ToString(), Payload.MessageTag.ToString());
		return;
	}

	if (NewWidget->Implements<UActorParameterWidgetInterface>() && Payload.RelevantActor.IsValid())
	{
		IActorParameterWidgetInterface::Execute_AddActorParameter(NewWidget, Payload.RelevantActor.Get());
	}
}

void UWitchForestLayout::ReceiveNotification(FGameplayTag Channel, const FWitchForestMessage& Payload)
{
	if (!NotificationWidgetClass)
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "WitchtForestLayout '{Name}' received a notification with GameplayTag '{TagName}', but no notification widget was found.", GetName(), Payload.Data.ToString());
		return;
	}

	if (!NotificationData)
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "WitchtForestLayout '{Name}' received a notification with GameplayTag '{TagName}', but it does not have a valid NotificationDataAsset.", GetName(), Payload.Data.ToString());
		return;
	}

	FNotificationDataAsset_Entry NotificationEntry;
	if (!NotificationData->FindDataFromTag(Payload.Data, NotificationEntry))
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "WitchtForestLayout '{Name}' received a notification with GameplayTag '{TagName}', but there was no corresponding entry in the NotificationData.", GetName(), Payload.Data.ToString());
		return;
	}

	UNotificationWidget* NotificationWidget = Cast<UNotificationWidget>(PushWidgetToLayer(NotificationWidgetClass, UI_SlotTag_Notification));
	if (!NotificationWidget)
	{
		return;
	}

	NotificationWidget->SetNotification(NotificationEntry);
}

UCommonActivatableWidget* UWitchForestLayout::PushWidgetToLayer(TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FGameplayTag LayerTag)
{
	if (WidgetClass.IsNull())
	{
		return nullptr;
	}

	if (WidgetClass.IsPending())
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "WitchForestLayout '{LayoutName}' creating widget of type '{WidgetClassName}'. The widget class was not loaded, loading synchronously.", GetName(), WidgetClass.ToString());
		WidgetClass.LoadSynchronous();
	}

	TObjectPtr<UCommonActivatableWidgetContainerBase> Layer = LayerMap.FindRef(LayerTag);
	if (!Layer)
	{
		UE_LOGFMT(LogWitchForestUI, Warning, "WitchForestLayout '{LayoutName}' failed to push widget '{WidgetClassName}' to layer. Could not find layer '{LayerName}'.", GetName(), WidgetClass.ToString(), LayerTag.ToString());
		return nullptr;
	}

	return Layer->AddWidget(WidgetClass.Get());
}
