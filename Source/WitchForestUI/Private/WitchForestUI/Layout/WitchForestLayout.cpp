// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/Layout/WitchForestLayout.h"

#include "CommonActivatableWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Dynamic/Interactable/ActorParameterWidgetInterface.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"

#include "WitchForestUI/Layout/WitchForestLayoutMap.h"

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
	if (NewWidget->Implements<UActorParameterWidgetInterface>() && Payload.RelevantActor.IsValid())
	{
		IActorParameterWidgetInterface::Execute_AddActorParameter(NewWidget, Payload.RelevantActor.Get());
	}
}

UCommonActivatableWidget* UWitchForestLayout::PushWidgetToLayer(TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FGameplayTag LayerTag)
{
	if (WidgetClass.IsNull())
	{
		return nullptr;
	}

	TObjectPtr<UCommonActivatableWidgetContainerBase> Layer = LayerMap.FindRef(LayerTag);
	if (!Layer)
	{
		return nullptr;
	}

	return Layer->AddWidget(WidgetClass.Get());
}
