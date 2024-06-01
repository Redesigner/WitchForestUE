// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h" 

#include "GameplayEffectTypes.h" 

#include "AreaOfEffectActor.generated.h"

class UEffectApplicationComponent;

UCLASS()
class WITCHFORESTABILITY_API AAreaOfEffectActor : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEffectApplicationComponent> EffectApplication;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEffectApplicationComponent> PeriodicEffectApplication;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayTags, meta = (AllowPrivateAccess = true))
	FGameplayTagContainer GameplayTags;


	TMap<FObjectKey, TArray<FActiveGameplayEffectHandle>> AppliedEffectMap;


	AAreaOfEffectActor();

	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	void Destroyed() override;

	void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	void RemoveEffectsFromActor(AActor* Actor);
};
