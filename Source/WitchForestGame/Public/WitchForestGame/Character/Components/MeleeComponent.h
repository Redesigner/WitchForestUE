// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeComponent.generated.h"


UENUM(BlueprintType)
enum EMeleeHitboxShape : uint8
{
	Box,
	Sphere,
	Capsule
};

USTRUCT(BlueprintType)
struct FMeleeHitboxSpawnParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMeleeHitboxShape> Shape = Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 1 || Shape == 2", EditConditionHides, ClampMin = 0.0f))
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 2", EditConditionHides, ClampMin = 0.0f))
	float HalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Shape == 0", EditConditionHides, ClampMin = 0.0f))
	FVector BoxExtents;
};


/*
USTRUCT()
struct WITCHFORESTGAME_API FMeleeHitboxHandle
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<UMeleeComponent> Owner;
};
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTGAME_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = true))
	FCollisionProfileName CollisionProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = true))
	bool bShowHitboxes = false;

	TArray<UShapeComponent*> SpawnedHitboxes;


public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorHit, AActor*)
	FOnActorHit OnActorHit;

	UMeleeComponent();

	// @TODO: finish writing handle class that can be used in place of a pointer type
	UShapeComponent* SpawnHitbox(FMeleeHitboxSpawnParameters SpawnParameters);

	void DestroyHitbox(UShapeComponent* Hitbox);

	// Function for binding to dynamic delegate for OnComponentBeginOverlap
	// Forwards to our OnActorHit Delegate
	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DestroyAllHitboxes();

};
