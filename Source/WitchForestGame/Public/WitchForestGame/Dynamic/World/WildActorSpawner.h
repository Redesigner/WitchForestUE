// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WildActorSpawner.generated.h"

class UBoxComponent;

UCLASS()
class WITCHFORESTGAME_API AWildActorSpawner : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> SpawnRegion;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess = true))
	bool bActive = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> SpawnClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess = true, ClampMin = 1))
	uint8 MaxSpawnAmount = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Spawning, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float SpawnDelay;

	uint8 CurrentlySpawnedAmount = 0;

	FTimerHandle RespawnTimer;


	AWildActorSpawner();

	void BeginPlay() override;

	void SpawnActor();

	FVector GetRandomSpawnPosition() const;

	UFUNCTION()
	void SpawnedActorDestroyed(AActor* SpawnedActor);
};
