// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "WitchPlayerController.generated.h"

class UInputMappingContext;
class UWitchForestASC;
class UHealthViewModel;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerController : public APlayerController, public IWitchForestTeamAgentInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	DECLARE_MULTICAST_DELEGATE(FOnPotentialAbilityActivationChanged);


	AWitchPlayerController();

	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;
	void BeginPlayingState() override;
	void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// Begin IWitchForestTeamAgentInterface
	void OverrideTeam(EWitchForestTeam NewTeam) override;
	void SetWitchForestTeam(EWitchForestTeam InTeam) override;
	EWitchForestTeam GetWitchForestTeam() const override;
	// End IWitchForestTeamAgentInterface

	void PotentialInteractionsChanged();

public:
	UFUNCTION(BlueprintCallable)
	UWitchForestASC* GetWitchForestASC() const;

	FOnPotentialAbilityActivationChanged OnPotentialAbilityActivationChanged;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetupUI(AWitchPlayerState* WitchPlayerState);
};
