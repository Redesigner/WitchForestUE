// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UBoxComponent;

UCLASS()
class WITCHFORESTGAME_API APortal : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> CollisionVolume;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Teleportation, meta = (AllowPrivateAccess = true))
	FVector Destination;
	
public:	
	APortal();

	UFUNCTION()
	void VolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
