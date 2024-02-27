// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Pickup/Potion.h"

#include "AbilitySystemInterface.h"

#include "WitchForestAbility/WitchForestASC.h"

void APotion::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (GetVelocity().SquaredLength() <= 1000.0f)
	{
		return;
	}
	if (!OtherComp)
	{
		return;
	}
	if (OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		Burst();
	}
}

void APotion::Burst()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(BurstDebounceTimer))
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Potion burst"));
	UWorld* World = GetWorld();
	check(World);

	if (!LastHolder.IsValid())
	{
		return;
	}
	if (!SplashEffect)
	{
		return;
	}

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(SplashRadius);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	World->OverlapMultiByChannel(OverlapResults, GetActorLocation(), GetActorQuat(), ECC_WorldDynamic, Sphere, CollisionQueryParams);

	// if draw flag
	DrawDebugSphere(World, GetActorLocation(), SplashRadius, 32, FColor::Red, false, 1.0f);
	TimerManager.SetTimer(BurstDebounceTimer, 0.5f, false);
	TArray<AActor*> HitActors;
	for (FOverlapResult OverlapResult : OverlapResults)
	{
		AActor* OverlappedActor = OverlapResult.GetActor();
		if (!OverlappedActor || HitActors.Contains(OverlappedActor))
		{
			continue;
		}
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OverlappedActor);
		if (!AbilitySystemInterface)
		{
			continue;
		}
		HitActors.Add(OverlappedActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();
		FGameplayEffectSpecHandle GameplayEffectSpec = LastHolder->MakeOutgoingSpec(SplashEffect, 0.0f, LastHolder->MakeEffectContext());
		LastHolder->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpec.Data.Get(), TargetASC);
	}
}
