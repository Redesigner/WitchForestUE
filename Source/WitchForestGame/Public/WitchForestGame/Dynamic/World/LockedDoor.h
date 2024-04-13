// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"
#include "LockedDoor.generated.h"

class UBoxComponent;

UCLASS()
class WITCHFORESTGAME_API ALockedDoor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> DoorShape;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Door, meta = (AllowPrivateAccess = true))
	bool bLocked = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Door, meta = (AllowPrivateAccess = true))
	bool bOpenWhenUnlocking = true;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Door, meta = (AllowPrivateAccess = true))
	bool bOpen = false;
	

	ALockedDoor();

	void Interact(AActor* Source) override;

	bool CanInteract(AActor* Source) const override;

public:
	void Unlock();

	void Open();

	void Close();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnlocked();

	UFUNCTION(BlueprintImplementableEvent)
	void OnOpened();

	UFUNCTION(BlueprintImplementableEvent)
	void OnClosed();
};
