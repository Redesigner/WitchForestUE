// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Effects/AreaOfEffectActor.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "WitchForestAbility/Effects/EffectApplicationComponent.h"

AAreaOfEffectActor::AAreaOfEffectActor()
{
	EffectApplication = CreateDefaultSubobject<UEffectApplicationComponent>(TEXT("EffectApplication"));
}

void AAreaOfEffectActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	FHitResult FakeHit = FHitResult(OtherActor, nullptr, GetActorLocation(), FVector());
	
	TArray<FActiveGameplayEffectHandle> AppliedEffects = EffectApplication->ApplyGameplayEffectsToTarget(OtherActor, FakeHit);
	if (AppliedEffects.IsEmpty())
	{
		return;
	}

	AppliedEffectMap.Add(OtherActor, AppliedEffects);
}

void AAreaOfEffectActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	RemoveEffectsFromActor(OtherActor);
}

void AAreaOfEffectActor::Destroyed()
{
	TSet<AActor*> AffectedActors;
	AppliedEffectMap.GetKeys(AffectedActors);
	for (AActor* AffectedActor : AffectedActors)
	{
		RemoveEffectsFromActor(AffectedActor);
	}
}

void AAreaOfEffectActor::RemoveEffectsFromActor(AActor* Actor)
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	TArray<FActiveGameplayEffectHandle>* EffectsToRemove = AppliedEffectMap.Find(Actor);
	if (!EffectsToRemove)
	{
		return;
	}

	for (FActiveGameplayEffectHandle& EffectToRemove : *EffectsToRemove)
	{
		ASC->RemoveActiveGameplayEffect(EffectToRemove);
	}
}
