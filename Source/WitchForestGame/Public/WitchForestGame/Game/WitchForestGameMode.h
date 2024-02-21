// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h" 

#include "Engine/StreamableManager.h" 


#include "WitchForestGameMode.generated.h"

class UItemSet;

UCLASS()
class WITCHFORESTGAME_API AWitchForestGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true))
	TObjectPtr<UItemSet> CurrentItemSet;

	// @TODO: Put this in a much safer place, like the Game Instance for example
	FStreamableManager StreamableManager;

public:
	AWitchForestGameMode();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UItemSet* GetItemSet() const;
};