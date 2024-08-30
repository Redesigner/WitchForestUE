// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"


UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};


// Every game needs one!
class WITCHFORESTGAME_API IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void Interact(AActor* Source);
	virtual void Interact_Implementation(AActor* Source) = 0;

	UFUNCTION(BlueprintNativeEvent)

	bool CanInteract(AActor* Source) const;
	virtual bool CanInteract_Implementation(AActor* Source) const { return true; }

	UFUNCTION(BlueprintNativeEvent)
	float GetRequiredHoldTime() const;
	virtual float GetRequiredHoldTime_Implementation() const { return 0.0f; }
};
