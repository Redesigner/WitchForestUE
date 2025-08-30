// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"

#include "GameplayAbilitySpec.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "ChangeTeamEffectComponent.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UChangeTeamEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Team, meta = (AllowPrivateAccess = true))
	TEnumAsByte<EWitchForestTeam> NewTeam = EWitchForestTeam::Witches;
	

	bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;

	void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo) const;

	void OnInhibitionChanged(FActiveGameplayEffectHandle ActiveGEHandle, bool bIsInhibited) const;


	void SetOverrideTeam(FActiveGameplayEffectHandle ActiveGEHandle) const;

	void EndOverrideTeam(FActiveGameplayEffectHandle ActiveGEHandle) const;
};
