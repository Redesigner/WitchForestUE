// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "GameplayTagContainer.h"

#include "WidgetLauncher.generated.h"

class UUserWidget;

UCLASS()
class WITCHFORESTGAME_API AWidgetLauncher : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Widget, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Widget, meta = (AllowPrivateAccess = true, Categories = "UI.MessageTag"))
	FGameplayTag WidgetMessageTag;

	void Interact_Implementation(AActor* Source) override;
};
