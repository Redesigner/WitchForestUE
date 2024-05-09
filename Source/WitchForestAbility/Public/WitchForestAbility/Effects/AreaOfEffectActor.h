// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GameplayEffectTypes.h" 

#include "AreaOfEffectActor.generated.h"

class UEffectApplicationComponent;

UCLASS()
class WITCHFORESTABILITY_API AAreaOfEffectActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEffectApplicationComponent> EffectApplication;

	TMap<AActor*, TArray<FActiveGameplayEffectHandle>> AppliedEffectMap;


	AAreaOfEffectActor();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	void Destroyed() override;

	void RemoveEffectsFromActor(AActor* Actor);
};
