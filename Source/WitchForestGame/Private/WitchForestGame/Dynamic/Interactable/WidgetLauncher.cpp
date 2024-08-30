// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/WidgetLauncher.h"

#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"

void AWidgetLauncher::Interact_Implementation(AActor* Source)
{
	APawn* Pawn = Cast<APawn>(Source);
	if (!Pawn)
	{
		return;
	}

	// We can't launch widgets on non-local controllers!
	// Unreal will catch this, but it's better to avoid here
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	if (!WidgetClass)
	{
		return;
	}

	UUserWidget* SpawnedWidget = CreateWidget(PlayerController, WidgetClass);
	if (!SpawnedWidget)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "WidgetLauncher '{LauncherName}' failed to launch widget. Widget of type '{ClassName}' was not created successfully.", GetName(), WidgetClass->GetName());
		return;
	}

	SpawnedWidget->AddToViewport();
}
