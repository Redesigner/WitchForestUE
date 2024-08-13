// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "GameplayEffect.h" 
#include "WitchForestGame/Messages/WitchForestMessage.h"

#include "WitchPlayerState.generated.h"

class UWitchForestASC;
class UWitchForestAbilitySet;
class UInventoryComponent;
class UBaseAttributeSet;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerState : public APlayerState, public IAbilitySystemInterface, public IWitchForestTeamAgentInterface
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
	TEnumAsByte<EWitchForestTeam> TeamId;

public:
	UFUNCTION(BlueprintCallable, Category = "WitchForest|PlayerState")
	UWitchForestASC* GetWitchForestASC() const { return AbilitySystem; }

	UFUNCTION(BlueprintCallable, Category = "WitchForest|PlayerState")
	UBaseAttributeSet* GetAttributeSet() const;

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "WitchForest|PlayerState")
	void ClientBroadcastMessage(const FWitchForestMessage Message);

	void GrantAbilities();

	/// Set our attributes to their default values;
	void InitializeAttributes();

	void OnAttributeSetDeath(FGameplayEffectSpec SpecCauser);

	bool IsAlive() const { return bAlive; }

	DECLARE_MULTICAST_DELEGATE(FOnDeath)
	FOnDeath OnDeath;

private:
	bool bAlive = false;

	bool bReceivingNotifications = false;

	bool bAbilitiesGranted = false;

	TQueue<FWitchForestMessage> UnprocessedNotifications;

	AWitchPlayerState();
	
	void BeginPlay() override;

	void FlushNotifications();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Begin IWitchForestTeamAgentInterface
	void OverrideTeam(EWitchForestTeam NewTeam) override;
	void SetWitchForestTeam(EWitchForestTeam InTeam) override;
	EWitchForestTeam GetWitchForestTeam() const override;
	// End IWitchForestTeamAgentInterface
};
