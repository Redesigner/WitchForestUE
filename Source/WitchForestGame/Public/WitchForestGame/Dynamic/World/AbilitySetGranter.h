// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySetGranter.generated.h"

class UWitchForestASC;
class UWitchForestAbilitySet;

UCLASS()
class WITCHFORESTGAME_API AAbilitySetGranter : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Abilities, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UWitchForestAbilitySet>> AbilitySets;


	AAbilitySetGranter();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void GrantAbilitiesTo(UWitchForestASC* ASC);
};
