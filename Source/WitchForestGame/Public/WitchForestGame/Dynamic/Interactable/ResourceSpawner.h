// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "ResourceSpawner.generated.h"

class UDropTableComponent;

UCLASS()
class WITCHFORESTGAME_API AResourceSpawner : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDropTableComponent> DropTableComponent;

public:	
	AResourceSpawner();

	void Interact(AActor* Source) override;
};
