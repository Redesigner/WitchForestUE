// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/WidgetLauncher.h"

#include "Blueprint/UserWidget.h"

void AWidgetLauncher::Interact(AActor* Source)
{
	APawn* Pawn = Cast<APawn>(Source);
	if (!Pawn)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!PlayerController)
	{
		return;
	}

	if (!WidgetClass)
	{
		return;
	}

	UUserWidget* SpawnedWidget = CreateWidget(PlayerController, WidgetClass);
	SpawnedWidget->AddToViewport();
}
