// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"
#include "DeathAbility.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTABILITY_API UDeathAbility : public UWitchForestGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability, meta = (AllowPrivateAccess = true))
	TSubclassOf<APawn> SpawnedPawnClass;
	
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)  override;

public:
	UDeathAbility(); 
};
