// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrowableSprout.generated.h"

class UBoxComponent;

UCLASS()
class WITCHFORESTGAME_API AGrowableSprout : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> PlatformStartLocation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> PlatformEndLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Growth, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float MoveTime = 2.0f;

	float CurrentMoveTime = 0.0f;


	bool bAlive = false;

	bool bExtended = false;

	bool bMoving = false;
	

	AGrowableSprout();

	void Tick(float Delta) override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void Grow();

	UFUNCTION()
	void ObjectEnterTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
