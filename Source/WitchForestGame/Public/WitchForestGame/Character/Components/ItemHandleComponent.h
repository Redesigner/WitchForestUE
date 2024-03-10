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

	UPROPERTY(ReplicatedUsing = OnRep_HeldItem, VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = true))
	TObjectPtr<APickup> HeldItem;

	TWeakObjectPtr<APickup> FakePickup;

	FWitchForestAbilitySet_GrantedHandles GrantedHandles;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_HeldItem(APickup* OldHeldItem);

	void DropAllItems();

public:
	TWeakObjectPtr<APickup> RequestedPickup;


	UItemHandleComponent();

	APickup* ConsumeItem();

	APickup* GetHeldItem() const { return HeldItem; }

	APickup* GetFakePickup() const { return FakePickup.Get(); }

	void SetFakePickup(APickup* Fake);

	void PickupItem(APickup* Item);

	bool HoldingItem() const;

	void SetHeldItemHidden(bool bHidden);
};
