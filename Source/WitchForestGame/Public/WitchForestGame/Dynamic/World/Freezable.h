// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Freezable.generated.h"


class UBoxComponent;

UCLASS()
class WITCHFORESTGAME_API AFreezable : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Freezing, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float FreezeTime = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	TObjectPtr<UBoxComponent> EndZone;

	FTimerHandle FreezeTimer;


	AFreezable();
	
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void Freeze();

	void Unfreeze();

	UFUNCTION()
	void ObjectEnterEndZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
