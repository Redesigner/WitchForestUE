// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/WidgetLauncher.h"

#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

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

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

	FWitchForestUIMessage NewMessage;
	NewMessage.MessageTag = WidgetMessageTag;
	MessageSystem.BroadcastMessage(WitchForestGameplayTags::MessageChannel_UI, NewMessage);

	//if (!WidgetClass)
	//{
	//	return;
	//}

	//UUserWidget* SpawnedWidget = CreateWidget(PlayerController, WidgetClass);
	//if (!SpawnedWidget)
	//{
	//	UE_LOGFMT(LogWitchForestGame, Error, "WidgetLauncher '{LauncherName}' failed to launch widget. Widget of type '{ClassName}' was not created successfully.", GetName(), WidgetClass->GetName());
	//	return;
	//}

	//SpawnedWidget->AddToViewport();
}
