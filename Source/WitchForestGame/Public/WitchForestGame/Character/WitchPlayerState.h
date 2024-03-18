// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffect.h" 

#include "WitchPlayerState.generated.h"

class UWitchForestASC;
class UWitchForestAbilitySet;
class UInventoryComponent;
class UBaseAttributeSet;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerState : public APlayerState, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestASC> AbilitySystem;

	// This should be set whenever the pawn type changes, but for now, this should be fine
	// It is also currently only loaded on construction, but should be able to change in the future
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UWitchForestAbilitySet>> AbilitySets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBaseAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryComponent> Inventory;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Behavior, meta = (AllowPrivateAccess = true))
	FGenericTeamId TeamId;


	bool bAlive = false;


	AWitchPlayerState();

	void GrantAbilities();
	
	void BeginPlay() override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FGenericTeamId GetGenericTeamId() const override;


public:
	UFUNCTION(BlueprintCallable, Category = "WitchForest|PlayerState")
	UWitchForestASC* GetWitchForestASC() const { return AbilitySystem; }

	UFUNCTION(BlueprintCallable, Category = "WitchForest|PlayerState")
	UBaseAttributeSet* GetAttributeSet() const;

	/// Set our attributes to their default values;
	void InitializeAttributes();

	void OnAttributeSetDeath(FGameplayEffectSpec SpecCauser);

	bool IsAlive() const { return bAlive; }

	DECLARE_MULTICAST_DELEGATE(FOnDeath)
	FOnDeath OnDeath;
};
