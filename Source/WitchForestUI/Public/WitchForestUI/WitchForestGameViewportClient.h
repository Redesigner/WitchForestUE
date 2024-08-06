// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "WitchForestGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTUI_API UWitchForestGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
	
	void ReceivedFocus(FViewport* Viewport) override;

	EMouseCaptureMode GetMouseCaptureMode() const override;
};
