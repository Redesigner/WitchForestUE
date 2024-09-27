// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "WitchForestMessage.generated.h"

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FWitchForestMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTag Verb;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	TObjectPtr<AActor> Source;

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	FGameplayTag Data;
};

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FWitchForestUIMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Gameplay, meta = (Categories = "UI.MessageTag"))
	FGameplayTag MessageTag;

	TWeakObjectPtr<AActor> RelevantActor;
};
