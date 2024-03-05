// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WitchPlayerController.generated.h"

class UInputMappingContext;
class UWitchForestASC;
class UHealthViewModel;

UCLASS()
class WITCHFORESTGAME_API AWitchPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;


	void OnPossess(APawn* InPawn) override;

	void BeginPlayingState() override;

public:
	AWitchPlayerController();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UWitchForestASC* GetWitchForestASC() const;

	void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetupUI(AWitchPlayerState* WitchPlayerState);
};
