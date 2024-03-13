// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"

#include "WitchForestGame/Dynamic/Pickup/Pickup.h"

#include "SoulSprite.generated.h"

class UAbilitySystemComponent;

UCLASS()
class WITCHFORESTGAME_API ASoulSprite : public APickup
{
	GENERATED_BODY()

	TWeakObjectPtr<UAbilitySystemComponent> OwningASC;


	bool CanBeStored() const override;

	void OnConsume() override;

public:
	ASoulSprite();

	void SetOwningASC(UAbilitySystemComponent* NewOwningASC);

};
