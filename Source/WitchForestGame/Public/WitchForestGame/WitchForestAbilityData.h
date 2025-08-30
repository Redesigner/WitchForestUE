// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WitchForestAbilityData.generated.h"

class UGameplayEffect;

UCLASS()
class WITCHFORESTGAME_API UWitchForestAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UWitchForestAbilityData& Get();

	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

};
