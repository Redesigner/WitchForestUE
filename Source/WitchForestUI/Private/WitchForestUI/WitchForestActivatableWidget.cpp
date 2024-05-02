// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestActivatableWidget.h"

#include "CommonInputSubsystem.h"

TOptional<FUIInputConfig> UWitchForestActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EWitchForestWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EWitchForestWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EWitchForestWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EWitchForestWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

void UWitchForestActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void UWitchForestActivatableWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UWitchForestActivatableWidget::SuspendOwningPlayerInput()
{
	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
	{
		FName SuspendToken = "BlockedByWidget";

		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, true);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, true);
	}
}

void UWitchForestActivatableWidget::ResumeOwningPlayerInput()
{
	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetOwningLocalPlayer()))
	{
		FName SuspendToken = "BlockedByWidget";

		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::MouseAndKeyboard, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Gamepad, SuspendToken, false);
		CommonInputSubsystem->SetInputTypeFilter(ECommonInputType::Touch, SuspendToken, false);
	}
}
