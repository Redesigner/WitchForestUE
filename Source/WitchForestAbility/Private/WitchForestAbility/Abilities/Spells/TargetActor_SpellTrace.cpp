// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/TargetActor_SpellTrace.h"

#include "Abilities/GameplayAbility.h"

ATargetActor_SpellTrace::ATargetActor_SpellTrace()
{
	PrimaryActorTick.bCanEverTick = true;
	StartLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
}

void ATargetActor_SpellTrace::Tick(float DeltaSeconds)
{
	if (SourceActor.IsValid() && SourceActor->GetLocalRole() != ENetRole::ROLE_SimulatedProxy)
	{
		FHitResult HitResult = PerformTrace();
		FVector EndPoint = HitResult.Component.IsValid() ? HitResult.ImpactPoint : HitResult.TraceEnd;

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			DrawDebugLine(GetWorld(), SourceActor->GetActorLocation(), EndPoint, FColor::Green, false);
			DrawDebugSphere(GetWorld(), EndPoint, 16, 10, FColor::Green, false);
		}
#endif // ENABLE_DRAW_DEBUG

		SetActorLocationAndRotation(EndPoint, SourceActor->GetActorRotation());
	}
}

void ATargetActor_SpellTrace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	if (!SourceActor.IsValid())
	{
		return;
	}

	WorldOffset = SourceActor->GetActorForwardVector().GetSafeNormal2D() * InitialTargetingDistance;
}

void ATargetActor_SpellTrace::ConfirmTargetingAndContinue()
{
	bDebug = false;
	FGameplayAbilityTargetDataHandle Handle = MakeTargetData(PerformTrace());
	TargetDataReadyDelegate.Broadcast(Handle);
}

FGameplayAbilityTargetDataHandle ATargetActor_SpellTrace::MakeTargetData(const FHitResult& HitResult) const
{
	return StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
}

FHitResult ATargetActor_SpellTrace::PerformTrace()
{
	if (!SourceActor.IsValid())
	{
		return FHitResult();
	}

	FVector TraceStartLocation = SourceActor->GetActorLocation() + WorldOffset;
	TraceStartLocation.Z += TraceDistance / 2.0f;
	FVector EndLocation = TraceStartLocation;
	EndLocation.Z -= TraceDistance;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	FHitResult TraceResult;
	GetWorld()->LineTraceSingleByProfile(TraceResult, TraceStartLocation, EndLocation, TraceProfile.Name, CollisionQueryParams);
	return TraceResult;
}
