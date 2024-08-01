// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestRootWidget.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include "WitchForestGame/WitchForestGameplayTags.h"

void UWitchForestRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.RegisterListener(WitchForestGameplayTags::Event_Notification, this, &ThisClass::OnNotification);
}

void UWitchForestRootWidget::OnNotification(FGameplayTag Channel, const FWitchForestMessage& Payload)
{
}
