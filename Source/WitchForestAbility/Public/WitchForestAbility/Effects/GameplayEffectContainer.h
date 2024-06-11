// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"
#include "GameplayEffect.h"

#include "GameplayEffectContainer.generated.h"

USTRUCT(BlueprintType)
struct FGameplayEffectMagnitudePair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Categories = "SetByCaller"))
	FGameplayTag SetByCallerTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Magnitude;
};

USTRUCT(BlueprintType)
struct FGameplayEffectMagnitudeEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UGameplayEffect> Effect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayEffectMagnitudePair> Magnitudes;
};

USTRUCT(BlueprintType)
struct WITCHFORESTABILITY_API FGameplayEffectContainer
{
	GENERATED_BODY()
	
	TArray<FGameplayEffectSpecHandle> MakeEffectSpecs(UGameplayAbility* OwningAbility) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Effects)
	TArray<FGameplayEffectMagnitudeEntry> Effects;
};
