// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestGame/Dynamic/World/DaytimeActor.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "DaytimeDoor.generated.h"

class UBoxComponent;

UCLASS()
class WITCHFORESTGAME_API ADaytimeDoor : public ADaytimeActor, public IInteractableInterface
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> InteractionVolume;

	ADaytimeDoor();

	void DayEnd() override;
	void DayStart() override;

	void Interact_Implementation(AActor* Source) override;

};
