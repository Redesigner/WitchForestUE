// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "WitchPlayerState.generated.h"

class UWitchForestASC;
class UWitchForestAbilitySet;
class UInventoryComponent;
class UBaseAttributeSet;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerState : public APlayerState, public IAbilitySystemInterface
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

	void GrantAbilities();
	
	/// Set our attributes to their default values;
public:
	AWitchPlayerState();

	UFUNCTION(BlueprintCallable, Category = "WitchForest|PlayerState")
	UWitchForestASC* GetWitchForestASC() const { return AbilitySystem; }
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "WitchForest|PlayerState")
	UBaseAttributeSet* GetAttributeSet() const;

	void BeginPlay() override;

	void InitializeAttributes();	
};
