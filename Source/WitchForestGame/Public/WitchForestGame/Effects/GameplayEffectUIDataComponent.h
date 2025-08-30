// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"

#include "GameplayAbilitySpec.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "GameplayEffectUIDataComponent.generated.h"

USTRUCT(BlueprintType)
struct WITCHFORESTGAME_API FWitchForestGameplayEffectUIData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = UI, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> Icon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = UI, meta = (AllowPrivateAccess = true))
	FText EffectDisplayName;
};
UCLASS()
class WITCHFORESTGAME_API UGameplayEffectUIDataComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	FWitchForestGameplayEffectUIData GetUIData() const { return UIData; }

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FWitchForestGameplayEffectUIData UIData;
};
