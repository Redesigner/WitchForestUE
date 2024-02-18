// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"

#include "ItemHandleComponent.generated.h"

class APickup;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTGAME_API UItemHandleComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<APickup> HeldItem;

	FWitchForestAbilitySet_GrantedHandles GrantedHandles;

public:	
	UItemHandleComponent();

	APickup* ConsumeItem();

	void PickupItem(APickup* Item);

	bool HoldingItem() const;
};
