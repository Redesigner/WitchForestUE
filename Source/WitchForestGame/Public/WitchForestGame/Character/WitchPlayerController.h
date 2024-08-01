// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestGame/Dynamic/Brewing/PotionRecipeSet.h"

#include "WitchPlayerController.generated.h"

class UInputMappingContext;
class UWitchForestASC;
class UHealthViewModel;
class UUserWidget;
class UMaterialParameterCollection;
class IBlendableInterface;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerController : public APlayerController, public IWitchForestTeamAgentInterface
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnPotentialAbilityActivationChanged);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> RootWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	TScriptInterface<IBlendableInterface> BlindEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMaterialParameterCollection> EffectParameterCollection;


	TWeakObjectPtr<UUserWidget> RootWidget;

	bool bIsBlind = false;

	AWitchPlayerController();

	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;
	void BeginPlayingState() override;
	void Tick(float DeltaSeconds) override;
	void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// Begin IWitchForestTeamAgentInterface
	void OverrideTeam(EWitchForestTeam NewTeam) override;
	void SetWitchForestTeam(EWitchForestTeam InTeam) override;
	EWitchForestTeam GetWitchForestTeam() const override;
	// End IWitchForestTeamAgentInterface

	void PotentialInteractionsChanged();

	void SpawnHUD();

	// GameplayMessageSubsystem Event Handlers
	void OnDiscoveryMessage(FGameplayTag Channel, const FWitchForestMessage& Payload);

	void HandleRecipeDiscoveredMessage(const FWitchForestMessage& Payload);

	void BlindStacksChanged(const FGameplayTag Tag, int32 Count);

public:
	UFUNCTION(BlueprintCallable)
	UWitchForestASC* GetWitchForestASC() const;

	FOnPotentialAbilityActivationChanged OnPotentialAbilityActivationChanged;

	UUserWidget* GetRootWidget() const { return RootWidget.Get(); }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetupUI(AWitchPlayerState* WitchPlayerState);

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayRecipeDiscoveredMessage(FPotionRecipe Recipe, FInventoryItemData Data);
};
