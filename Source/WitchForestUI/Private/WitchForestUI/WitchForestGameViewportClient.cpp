// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/WitchForestGameViewportClient.h"

void UWitchForestGameViewportClient::ReceivedFocus(FViewport* ViewportFocused)
{
	Super::ReceivedFocus(ViewportFocused);
}

EMouseCaptureMode UWitchForestGameViewportClient::GetMouseCaptureMode() const
{
	return EMouseCaptureMode::NoCapture;
}
