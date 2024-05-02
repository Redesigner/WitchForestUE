// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WitchForestActivatableWidget.generated.h"


UENUM(BlueprintType)
enum class EWitchForestWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

UCLASS()
class WITCHFORESTUI_API UWitchForestActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Input)
	EWitchForestWidgetInputMode InputConfig = EWitchForestWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	
	TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	void NativeOnActivated() override;
	void NativeOnDeactivated() override;

	void SuspendOwningPlayerInput();

	void ResumeOwningPlayerInput();
};
