// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "WitchPlayerController.generated.h"

class UInputMappingContext;
class UWitchForestASC;
class UHealthViewModel;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;


	AWitchPlayerController();

	void BeginPlay() override;

	void OnPossess(APawn* InPawn) override;

	void BeginPlayingState() override;

	void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;


	// IGenericTeamAgentInterface
	FGenericTeamId GetGenericTeamId() const override;
	// End IGenericTeamAgentInterface

public:
	UFUNCTION(BlueprintCallable)
	UWitchForestASC* GetWitchForestASC() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetupUI(AWitchPlayerState* WitchPlayerState);
};
